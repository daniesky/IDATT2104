use std::thread;

fn main() {
    let mut threads = Vec::new();

    for i in 0..10 {
        threads.push(thread::spawn(move || {
            // i is copied into thread
            println!("Output from thread {}", i);
        }));
    }

    for thread in threads {
        let _ = thread.join(); // let _ means that the return value should be ignored
    }
}
