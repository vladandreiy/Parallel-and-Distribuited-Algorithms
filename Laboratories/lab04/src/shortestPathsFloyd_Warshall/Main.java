package shortestPathsFloyd_Warshall;

public class Main {
    static int M = 9;
    static int P = 4;
    static int[][] graph = {{0, 1, M, M, M},
            {1, 0, 1, M, M},
            {M, 1, 0, 1, 1},
            {M, M, 1, 0, M},
            {M, M, 1, M, 0}};
    static int N = graph.length;

    public static void main(String[] args) {
        // Parallelize me (You might want to keep the original code in order to compare)
//        for (int k = 0; k < 5; k++) {
//            for (int i = 0; i < 5; i++) {'
//                for (int j = 0; j < 5; j++) {
//                    graph[i][j] = Math.min(graph[i][k] + graph[k][j], graph[i][j]);
//                }
//            }
//        }
        Thread [] threads  = new Thread[P];
        for (int i = 0; i < P; i++) {
            threads[i] = new Thread(new MyThread(i));
            threads[i].start();
        }
        for (Thread thread : threads) {
            try {
                thread.join();
            } catch (InterruptedException e) {
                e.printStackTrace();
            }
        }
        for (int i = 0; i < 5; i++) {
            for (int j = 0; j < 5; j++) {
                System.out.print(graph[i][j] + " ");
            }
            System.out.println();
        }
    }
}


class MyThread implements Runnable {
    int id;
    private int start;
    private int end;

    public MyThread(int id) {
        this.id = id;
        start = (int) (id * (double) Main.N / Main.P);
        end = ((id + 1) * (double) Main.N / Main.P) < Main.N ?
                (int) ((id + 1) * (double) Main.N / Main.P) : Main.N;
    }

    @Override
    public void run() {
        for (int k = 0; k < 5; k++) {
            for (int i = start; i < end; i++) {
                for (int j = 0; j < 5; j++) {
                    Main.graph[i][j] = Math.min(Main.graph[i][k] + Main.graph[k][j], Main.graph[i][j]);
                }
            }
        }
    }
}