package synchronizedSortedList;

import java.io.File;
import java.io.FileNotFoundException;
import java.util.ArrayList;
import java.util.Collections;
import java.util.List;
import java.util.Scanner;
import java.util.concurrent.Semaphore;

public class Reader extends Thread {
    private final String filename;

    private final List<Integer> list;

    public Reader(String filename, List<Integer> list) {
        super();
        this.filename = filename;
        this.list = list;
    }

    @Override
    public void run() {
        try {
            Scanner scanner = new Scanner(new File(filename));
            while (scanner.hasNextInt()) {
                list.add(scanner.nextInt());
            }
            Main.semaphore.release();
        } catch (FileNotFoundException e) {
            e.printStackTrace();
        }
    }
}
