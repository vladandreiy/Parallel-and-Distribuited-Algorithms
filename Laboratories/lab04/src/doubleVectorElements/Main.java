package doubleVectorElements;

public class Main {
    static int[] v;
    static int N = 100000013;
    static int P = 8;

    public static void main(String[] args) {
        v = new int[N];

        for (int i = 0; i < N; i++) {
            v[i] = i;
        }

        Thread[] threads = new Thread[P];

        for (int i = 0; i < P; i++) {
            threads[i] = new Thread(new MyVector(i));
            threads[i].start();
        }

        for (Thread thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }

        for (int i = 0; i < N; i++) {
            if (v[i] != i * 2) {
                System.out.println("Wrong answer");
                System.exit(1);
            }
        }
        System.out.println("Correct");
    }
}

class MyVector implements Runnable {
    int id;
    private int start;
    private int end;

    public MyVector(int id) {
        this.id = id;
        start = (int) (id * (double) Main.N / Main.P);
        end = ((id + 1) * (double) Main.N / Main.P) < Main.N ?
                (int) ((id + 1) * (double) Main.N / Main.P) : Main.N;
    }

    @Override
    public void run() {
        for (int i = start; i < end; i++) {
            Main.v[i] *= 2;
        }
    }
}
