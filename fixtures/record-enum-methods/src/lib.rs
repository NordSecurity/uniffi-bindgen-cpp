#[derive(uniffi::Error, Debug, thiserror::Error)]
pub enum ShapeError {
    #[error("negative scale factor: {factor}")]
    NegativeScale { factor: f64 },
}

#[derive(uniffi::Record, Clone, Debug, PartialEq)]
pub struct Point {
    pub x: i32,
    pub y: i32,
}

#[uniffi::export]
impl Point {
    pub fn magnitude_squared(&self) -> i64 {
        (self.x as i64) * (self.x as i64) + (self.y as i64) * (self.y as i64)
    }

    pub fn translated(&self, dx: i32, dy: i32) -> Point {
        Point {
            x: self.x + dx,
            y: self.y + dy,
        }
    }

    pub fn label(&self) -> String {
        format!("({}, {})", self.x, self.y)
    }
}

#[derive(uniffi::Enum, Clone, Debug, PartialEq)]
pub enum Shape {
    Circle { radius: f64 },
    Rectangle { width: f64, height: f64 },
}

#[uniffi::export]
impl Shape {
    pub fn area(&self) -> f64 {
        match self {
            Shape::Circle { radius } => std::f64::consts::PI * radius * radius,
            Shape::Rectangle { width, height } => width * height,
        }
    }

    /// Fallible, to check that throwing methods work on enums too.
    pub fn scaled(&self, factor: f64) -> Result<Shape, ShapeError> {
        if factor < 0.0 {
            return Err(ShapeError::NegativeScale { factor });
        }
        Ok(match self {
            Shape::Circle { radius } => Shape::Circle {
                radius: radius * factor,
            },
            Shape::Rectangle { width, height } => Shape::Rectangle {
                width: width * factor,
                height: height * factor,
            },
        })
    }

    pub fn describe(&self) -> String {
        match self {
            Shape::Circle { radius } => format!("circle r={radius}"),
            Shape::Rectangle { width, height } => format!("rectangle {width}x{height}"),
        }
    }
}

#[derive(uniffi::Enum, Clone, Debug)]
pub enum Flag {
    On,
    Off,
}

#[uniffi::export]
impl Flag {
    pub fn inverted(&self) -> Flag {
        match self {
            Flag::On => Flag::Off,
            Flag::Off => Flag::On,
        }
    }
}

#[uniffi::export]
pub trait Zeta: Send + Sync {
    fn zeta(&self) -> i32;
}

#[derive(uniffi::Object)]
pub struct Alpha;

#[uniffi::export]
impl Alpha {
    #[uniffi::constructor]
    pub fn new() -> Self {
        Alpha
    }
}

#[uniffi::export]
impl Zeta for Alpha {
    fn zeta(&self) -> i32 {
        42
    }
}

#[uniffi::export]
pub fn make_zeta() -> std::sync::Arc<dyn Zeta> {
    std::sync::Arc::new(Alpha)
}

uniffi::setup_scaffolding!("record_enum_methods");
