use libc::{__u32, __u8, c_int, size_t};

mod improved;
mod naive;
mod types;
mod utils;

/// For C++ FFI
#[no_mangle]
pub extern "C" fn adder(nrow: size_t, ncol: size_t) {
    println!("{}", nrow + ncol)
}
