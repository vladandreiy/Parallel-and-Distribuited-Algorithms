package multipleProducersMultipleConsumersNBuffer;

import java.util.Queue;

public class Buffer {
    
    Queue<Integer> queue;
    int limit;
    
    public Buffer(int size) {
        limit = size;
        queue = new LimitedQueue<>(size);
    }

	public void put(int value) {
        synchronized (this) {
            while (queue.size() == limit) {
                try {
                    wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            queue.add(value);
            notifyAll();
        }
	}

	public int get() {
        synchronized (this) {
            while (queue.isEmpty()) {
                try {
                    wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            int a = -1;
            Integer result = queue.poll();
            if (result != null) {
                a = result;
            }
            notifyAll();
            return a;
        }
	}
}
