use libc::{__u32, __u8, c_char, c_int, size_t};

mod improved;
mod naive;
mod types;
mod utils;

use std::ffi::CString;

use std::slice;

/// For C++ FFI
#[no_mangle]
pub extern "C" fn adder(nrow: size_t, ncol: size_t) {
    println!("{}", nrow + ncol)
}

/// For C++ FFI
#[no_mangle]
pub extern "C" fn read_files(
    w_path: *const c_char,
    w_path_len: size_t,
    q_path: *const c_char,
    q_path_len: size_t,
) {
    let w_path = unsafe { slice::from_raw_parts(w_path, w_path_len) };
    let q_path = unsafe { slice::from_raw_parts(q_path, q_path_len) };

    let mut w = String::new();
    let mut q = String::new();

    for ch in w_path {
        w.push(*ch as u8 as char)
    }

    for ch in q_path {
        q.push(*ch as u8 as char)
    }

    println!("{} {}", w, q)
}

/// For C++ FFI
#[no_mangle]
pub extern "C" fn get_str() -> *const c_char {
    let s = CString::new("ciao").unwrap();
    let p = s.as_ptr();
    std::mem::forget(s);
    p
}

#[no_mangle]
pub extern "C" fn get_strs() -> *const *const u8 {
    let v = vec!["JebusHello\0".as_ptr(), "World\0".as_ptr()];
    let k = v.as_ptr();
    std::mem::forget(v);
    k
}

/// For C++ FFI
#[repr(C)]
#[derive(Debug)]
pub struct EdString {
    pub data: *const *const u8,
    pub metadata: *const size_t,
}

#[no_mangle]
pub extern "C" fn get_ed_string() -> EdString {
    // data
    let v = vec!["Hello\0".as_ptr(), "World\0".as_ptr()];
    let k = v.as_ptr();
    std::mem::forget(v);

    // metadata
    let c: Vec<usize> = vec![1, 1];
    let j = c.as_ptr();
    std::mem::forget(c);

    let o = EdString {
        data: k,
        metadata: j,
    };

    // println!("{:p}", o.data);

    o
}
