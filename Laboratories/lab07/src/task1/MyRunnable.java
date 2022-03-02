package task1;

import java.io.File;
import java.util.ArrayList;
import java.util.concurrent.ExecutorService;
import java.util.concurrent.atomic.AtomicInteger;

public class MyRunnable implements Runnable {
    private final ArrayList<Integer> partialPath;
    private final int destination;
    private final ExecutorService tpe;
    private final AtomicInteger inQueue;

    public MyRunnable(ArrayList<Integer> partialPath, int destination, ExecutorService tpe, AtomicInteger inQueue) {
        this.partialPath = partialPath;
        this.destination = destination;
        this.tpe = tpe;
        this.inQueue = inQueue;
    }

    @Override
    public void run() {
        if (partialPath.get(partialPath.size() - 1) == destination) {
            System.out.println(partialPath);
            int left = inQueue.decrementAndGet();
            if (left == 0) {
                tpe.shutdown();
            }
            return;
        }

        // se verifica nodurile pentru a evita ciclarea in graf
        int lastNodeInPath = partialPath.get(partialPath.size() - 1);
        for (int[] ints : Main.graph) {
            if (ints[0] == lastNodeInPath) {
                if (partialPath.contains(ints[1]))
                    continue;
                ArrayList<Integer> newPartialPath = new ArrayList<>(partialPath);
                newPartialPath.add(ints[1]);
                inQueue.incrementAndGet();
                tpe.submit(new MyRunnable(newPartialPath, destination, tpe, inQueue));
            }
        }

        int left = inQueue.decrementAndGet();
        if (left == 0) {
            tpe.shutdown();
        }
    }
}
