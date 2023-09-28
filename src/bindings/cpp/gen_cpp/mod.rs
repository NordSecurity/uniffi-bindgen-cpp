mod compounds;
mod enum_;
mod filters;
mod miscellany;
mod object;
mod primitives;
mod record;

use std::{borrow::Borrow, collections::HashMap};

use anyhow::{Context, Result};
use askama::Template;
use serde::{Deserialize, Serialize};
use uniffi_bindgen::{interface::{AsType, Type}, BindingsConfig, ComponentInterface};

#[derive(Clone, Deserialize, Serialize)]
pub(crate) struct Config {
    cdylib_name: Option<String>,
}

impl BindingsConfig for Config {
    const TOML_KEY: &'static str = "cpp";

    fn update_from_ci(&mut self, ci: &ComponentInterface) {
        self.cdylib_name.get_or_insert_with(|| format!("uniffi_{}", ci.namespace()));
    }

    fn update_from_cdylib_name(&mut self, cdylib_name: &str) {
        self.cdylib_name.get_or_insert_with(|| cdylib_name.to_string());
    }

    fn update_from_dependency_configs(&mut self, _config_map: HashMap<&str, &Self>) {}
}

#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "types.cpp")]
struct TypeRenderer<'a> {
    ci: &'a ComponentInterface,
}

#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "scaffolding.hpp")]
struct ScaffoldingHeader<'a> {
    ci: &'a ComponentInterface,
}

impl<'a> ScaffoldingHeader<'a> {
    fn new(ci: &'a ComponentInterface) -> Self {
        Self { ci }
    }
}

#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "wrapper.hpp")]
struct CppWrapperHeader<'a> {
    ci: &'a ComponentInterface,
}

impl<'a> CppWrapperHeader<'a> {
    fn new(ci: &'a ComponentInterface) -> Self {
        Self { ci }
    }

    pub(crate) fn sorted_records(
        &self,
        types: impl Iterator<Item = &'a Type>,
    ) -> impl Iterator<Item = &'a Type> {
        let mut pod_types: Vec<&'a Type> = vec![];
        let mut rest_types: Vec<&'a Type> = vec![];

        for typ in types {
            if let Type::Record(name) = typ {
                if let Some(rec) = self.ci.get_record_definition(name) {
                    let contains_recs = rec
                        .fields()
                        .iter()
                        .any(|field| matches!(field.as_type(), Type::Record(_)));
                    if contains_recs {
                        rest_types.push(typ);
                    } else {
                        pod_types.push(typ);
                    }
                }
            } else {
                continue
            }
        }

        pod_types.into_iter().chain(rest_types)
    }
}

#[derive(Template)]
#[template(syntax = "cpp", escape = "none", path = "wrapper.cpp")]
struct CppWrapper<'a> {
    ci: &'a ComponentInterface,
    config: Config,
    type_helper_code: String,
}

impl<'a> CppWrapper<'a> {
    pub(crate) fn new(ci: &'a ComponentInterface, config: Config) -> Self {
        Self {
            ci,
            config,
            type_helper_code: TypeRenderer { ci }.render().unwrap(),
        }
    }
}

pub(crate) struct Bindings {
    pub(crate) scaffolding_header: String,
    pub(crate) header: String,
    pub(crate) source: String,
}

pub(crate) fn generate_cpp_bindings(
    ci: &ComponentInterface,
    config: Config,
) -> Result<Bindings> {
    let scaffolding_header = ScaffoldingHeader::new(ci)
        .render()
        .context("generating scaffolding header failed")?;
    let header = CppWrapperHeader::new(ci)
        .render()
        .context("generating C++ bindings header failed")?;
    let source = CppWrapper::new(ci, config)
        .render()
        .context("generating C++ bindings failed")?;

    Ok(Bindings { scaffolding_header, header, source })
}
