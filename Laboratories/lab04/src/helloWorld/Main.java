package helloWorld;

public class Main {
    public static void main(String[] args) {
        Thread[] threads = new Thread[Runtime.getRuntime().availableProcessors()];
        for (Thread thread : threads) {
            thread = new Thread(new MyThread());
            thread.start();
        }
        for (Thread thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
    }
}

class MyThread implements Runnable {
    int id;
    @Override
    public void run() {
        System.out.println("Hello from thread #" + Thread.currentThread().getId());
    }
}