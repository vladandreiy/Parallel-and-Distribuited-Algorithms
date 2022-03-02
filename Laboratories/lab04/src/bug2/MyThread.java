package bug2;

/**
 * Why does this code not block? We took the same lock twice!
 */
public class MyThread implements Runnable {
    /*
    Metodele și blocurile de cod synchronized din Java sunt reentrante. Dacă un thread a obținut
    monitorul unui obiect, atunci el va putea intra în orice alt bloc și metodă sincronizate ce
    sunt asociate cu acel obiect (implicit cu acel monitor).
     */
    static int i;

    @Override
    public synchronized void run() {
        synchronized (this) {
            synchronized (this) {
                i++;
            }
        }
    }
}
