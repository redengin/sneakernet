fn main() {
    println!("cargo:rustc-link-arg-bins=-Tlinkall.x");

    // wifi support wifi
    println!("cargo::rustc-link-arg=-Trom_functions.x");
}