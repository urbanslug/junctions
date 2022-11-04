use libc::{__u32, __u8, c_char, c_int, size_t};

mod improved;
mod naive;
mod types;
mod utils;

use eds::EDT;
use std::ffi::CString;
use std::fs::Metadata;
use std::slice;

#[repr(C)]
#[derive(Debug)]
pub struct EdString {
    pub data: *const *const c_char,
    pub metadata: *const size_t,
    pub metadata_len: size_t,
}

/// For C++ FFI
#[no_mangle]
pub extern "C" fn pass_string() -> *const *mut c_char {
    let strs = vec!["Hw", "Cow", "mister", "bev"];
    let strings: Vec<String> = strs.iter().map(|s| String::from(*s)).collect();

    let data: Vec<CString> = strings
        .iter()
        .map(|s: &String| CString::new(s.as_bytes()).unwrap())
        .collect();

    let data: Vec<*mut c_char> = data
        .iter()
        .map(|s: &CString| s.clone().into_raw())
        .collect();

    // c_char is i8
    let data_ptr: *const *mut c_char = data.as_ptr();

    std::mem::forget(data);

    data_ptr
}

/// For C++ FFI
#[no_mangle]
pub extern "C" fn read_ed_string(
    raw_ed_string: *const c_char,
    raw_ed_string_len: size_t,
) -> EdString {
    let raw_ed_string = unsafe { slice::from_raw_parts(raw_ed_string, raw_ed_string_len) };
    let mut eds_rs = String::new();

    for ch in raw_ed_string {
        eds_rs.push(*ch as u8 as char)
    }

    let edt = EDT::from_str(&eds_rs);
    let properties_edt: types::EdtSets = utils::iterate_sets(&edt);

    let metadata = properties_edt.iter().map(|e| e.3).collect::<Vec<usize>>();

    let metadata_len: size_t = properties_edt.len();

    let data = (0..properties_edt.len())
        .map(|idx| utils::set_members(&edt, &properties_edt, idx))
        .flatten()
        .collect::<Vec<String>>();

    eprintln!("[junctions::lib::read_ed_string]");

    eprintln!("\t{:?}\n\t{:?}\n\t{}", data, metadata, metadata_len);

    let strs = vec!["Hw", "Cow", "mister", "bev"];

    let data: Vec<CString> = strs
        .iter()
        .map(|s: &&str| CString::new(*s).unwrap())
        .collect();

    let data: Vec<*const c_char> = data.iter().map(|s: &CString| s.as_ptr()).collect();

    // c_char is i8
    let data_ptr: *const *const c_char = data.as_ptr();

    /*
    let data = vec![
        "Hello\0".as_ptr(),
        "World\0".as_ptr(),
        "Cow\0".as_ptr(),
        "Chicken\0".as_ptr(),
    ];
    */

    // let data_ptr = data.as_ptr();
    std::mem::forget(data);

    // metadata
    let metadata: Vec<size_t> = Vec::from([2, 1, 1]);

    let metadata_len = metadata.len();

    let metadata_ptr = metadata.as_ptr();
    std::mem::forget(metadata);

    // println!("{:p} {:p}", metadata_ptr, data_ptr);

    EdString {
        data: data_ptr,
        metadata: metadata_ptr,
        metadata_len,
    }
}
