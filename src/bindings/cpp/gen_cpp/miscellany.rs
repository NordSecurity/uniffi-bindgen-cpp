use uniffi_bindgen::backend::{CodeType, Literal};
use paste::paste;

macro_rules! impl_code_type_for_miscellany {
    ($T:ty, $canonical_name:literal) => {
        paste! {
            #[derive(Debug)]
            pub(crate) struct $T;

            impl CodeType for $T  {
                fn type_label(&self) -> String {
                    format!("{}", $canonical_name)
                }

                fn canonical_name(&self) -> String {
                    format!("{}", $canonical_name)
                }

                fn literal(&self, _literal: &Literal) -> String {
                    unreachable!()
                }
            }
        }
    };
}

impl_code_type_for_miscellany!(TimestampCodeType, "std::chrono::time_point");
impl_code_type_for_miscellany!(DurationCodeType, "std::chrono::duration");
