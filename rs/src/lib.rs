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

#[repr(C)]
#[derive(Debug)]
pub struct EEdString {
    pub data: *const *const u8,
    pub metadata: *const size_t,
    pub metadata_len: size_t,
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

    EdString {
        data: k,
        metadata: j,
    }
}

/// For C++ FFI
#[no_mangle]
pub extern "C" fn read_ed_string(w_path: *const c_char, w_path_len: size_t) -> EEdString {
    let w_path = unsafe { slice::from_raw_parts(w_path, w_path_len) };
    let mut w = String::new();

    for ch in w_path {
        w.push(*ch as u8 as char)
    }

    eprintln!("[junctions::lib::read_ed_string]");

    let strs = vec!["Hw", "mister", "bev"];

    let v: Vec<CString> = strs
        .iter()
        .map(|s: &&str| CString::new(*s).unwrap())
        .collect();

    //
    let data = vec![
        "Hello\0".as_ptr(),
        "World\0".as_ptr(),
        "Cow\0".as_ptr(),
        "Chicken\0".as_ptr(),
    ];

    let data_ptr = data.as_ptr();
    std::mem::forget(data);

    // metadata
    let metadata: Vec<size_t> = Vec::from([2, 1, 1]);

    let l = metadata.len();

    let metadata_ptr = metadata.as_ptr();
    std::mem::forget(metadata);

    // println!("{:p} {:p}", metadata_ptr, data_ptr);

    EEdString {
        data: data_ptr,
        metadata: metadata_ptr,
        metadata_len: l,
    }
}
