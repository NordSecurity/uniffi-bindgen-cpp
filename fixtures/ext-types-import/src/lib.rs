use std::{collections::HashMap, sync::Arc};

use uniffi_ext_types_export::{Coordinates, Counter, Direction, Greeter, Handle, Shape, Vehicle};

pub fn roundtrip_coord(coord: Coordinates) -> Coordinates {
    coord
}

pub fn roundtrip_coords(coords: Vec<Coordinates>) -> Vec<Coordinates> {
    coords
}

pub fn roundtrip_direction(dir: Direction) -> Direction {
    dir
}

pub fn roundtrip_handle(h: Handle) -> Handle {
    h
}

pub fn roundtrip_counter(c: Arc<Counter>) -> Arc<Counter> {
    c
}

pub struct Counters {
    pub counter: Arc<Counter>,
    pub label: u32,
}

pub fn roundtrip_counters(c: Counters) -> Counters {
    c
}

pub fn call_greeter(g: Arc<dyn Greeter>) -> String {
    g.greet()
}

pub fn roundtrip_opt_coord(c: Option<Coordinates>) -> Option<Coordinates> {
    c
}

pub fn roundtrip_opt_counter(c: Option<Arc<Counter>>) -> Option<Arc<Counter>> {
    c
}

pub fn roundtrip_shape(s: Shape) -> Shape {
    s
}

pub enum InnerShape {
    Empty,
    Hold { shape: Shape },
}

pub fn roundtrip_inner(i: InnerShape) -> InnerShape {
    i
}

pub fn roundtrip_coord_map(
    locations: HashMap<String, Coordinates>,
) -> HashMap<String, Coordinates> {
    locations
}

pub fn roundtrip_counter_list(cs: Vec<Arc<Counter>>) -> Vec<Arc<Counter>> {
    cs
}

pub fn vehicle_year(v: Arc<dyn Vehicle>) -> i32 {
    v.year()
}

uniffi::include_scaffolding!("ext_types_import");
