use crate::bindings::cpp::CodeType;
use paste::paste;
use uniffi_bindgen::{backend::Literal, ComponentInterface};

macro_rules! impl_code_type_for_miscellany {
    ($T:ty, $label:literal, $canonical_name:literal) => {
        paste! {
            #[derive(Debug)]
            pub(crate) struct $T;

            impl CodeType for $T  {
                fn type_label(&self, _ci: &ComponentInterface) -> String {
                    format!("{}", $label)
                }

                fn canonical_name(&self) -> String {
                    format!("{}", $canonical_name)
                }

                fn literal(&self, _literal: &Literal, _ci: &ComponentInterface) -> String {
                    unreachable!()
                }
            }
        }
    };
}

impl_code_type_for_miscellany!(
    TimestampCodeType,
    "std::chrono::time_point<std::chrono::system_clock, std::chrono::nanoseconds>",
    "Timestamp"
);
impl_code_type_for_miscellany!(
    DurationCodeType,
    "std::chrono::duration<int64_t, std::nano>",
    "Duration"
);
