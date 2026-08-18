#[derive(Debug, Clone)]
pub struct Coordinates {
    pub x: i32,
    pub y: i32,
}

#[derive(Debug, Clone)]
pub enum Direction {
    North,
    South,
}

pub struct Handle(pub i64);
uniffi::custom_newtype!(Handle, i64);

pub struct Counter {
    value: std::sync::atomic::AtomicU32,
}

impl Counter {
    fn new() -> Self {
        Counter {
            value: std::sync::atomic::AtomicU32::new(0),
        }
    }

    fn increment(&self) {
        self.value.fetch_add(1, std::sync::atomic::Ordering::SeqCst);
    }

    fn get(&self) -> u32 {
        self.value.load(std::sync::atomic::Ordering::SeqCst)
    }
}
pub trait Greeter: Send + Sync {
    fn greet(&self) -> String;
}

pub enum Shape {
    Circle { radius: f64 },
    Rectangle { width: f64, height: f64 },
}

pub trait Vehicle: Send + Sync {
    fn year(&self) -> i32;
}

pub struct Car {}

impl Vehicle for Car {
    fn year(&self) -> i32 {
        1984
    }
}

pub fn to_car() -> std::sync::Arc<dyn Vehicle> {
    std::sync::Arc::new(Car {})
}

uniffi::include_scaffolding!("ext_types_export");
