package multipleProducersMultipleConsumers;

public class Buffer {
    private int a;

    public Buffer() {
        this.a = -1;
    }

    void put(int value) {
        synchronized (this) {
            while (a != -1) {
                try {
                    wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }
            a = value;
            notifyAll();
        }
    }

    int get() {
        synchronized (this) {
            while (a == -1) {
                try {
                    wait();
                } catch (InterruptedException e) {
                    e.printStackTrace();
                }
            }

            int temp = a;
            a = -1;
            notifyAll();
            return temp;
        }
    }
}
