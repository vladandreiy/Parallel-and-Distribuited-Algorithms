import java.io.EOFException;
import java.io.IOException;
import java.io.RandomAccessFile;
import java.util.LinkedHashMap;
import java.util.Map;
import java.util.concurrent.Callable;

public class MapWorker implements Callable<MapResult> {
    private static final String letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    private final MapTask mapTask;

    public MapWorker(MapTask mapTask) {
        this.mapTask = mapTask;
    }

    @Override
    public MapResult call() throws Exception {
        return resolveMapTask();
    }

    public MapResult resolveMapTask() throws IOException {
        RandomAccessFile file = new RandomAccessFile(mapTask.getDocumentName(), "r");
        int bytesRead = 0;
        int currWordSize = 0;
        char currentChar = 0;
        boolean firstChar = true;

        // If it isn't the beginning of the text, check if there is a word that will be processed by the previous task
        if (mapTask.getOffset() > 0) {
            file.seek(mapTask.getOffset() - 1);
            bytesRead = -1;
            // If last word as not finished, it's not this task's job to finish it (read until the next delimiter)
            while ((firstChar || isLetter(currentChar)) && bytesRead < mapTask.getFragmentDim()) {
                try {
                    currentChar = (char) file.readByte();
                    firstChar = false;
                    bytesRead++;
                } catch (EOFException e) {
                    return new MapResult(mapTask.getDocumentName(), new LinkedHashMap<>(), null);
                }
            }
        }
        Map<Integer, Integer> dictionary = new LinkedHashMap<>();
        String longestWord = null;
        int lastDelimiter = mapTask.getOffset() + bytesRead;
        StringBuilder currentWord = new StringBuilder();

        // Task will be finished when the last word from the current offset has been read
        while (lastDelimiter < mapTask.getOffset() + mapTask.getFragmentDim()) {
            try {
                currentChar = (char) file.readByte();
                bytesRead++;
            } catch (EOFException e) {
                // If file ended, add last word to file and break from the loop
                if (longestWord == null)
                    // If longestWord has not been set, set it to currentWord
                    longestWord = currentWord.toString();
                else if (longestWord.length() < currentWord.length())
                    // If currentWord's length is higher than longestWord, set longestWord to currentWord
                    longestWord = currentWord.toString();
                if (dictionary.containsKey(currWordSize))
                    // If dictionary already contains currentWordSize, increment the number of words with currentWordSize
                    dictionary.put(currWordSize, dictionary.get(currWordSize) + 1);
                else
                    // If dictionary doesn't contain currentWordSize, set the number of words with currentWordSize to 1
                    dictionary.put(currWordSize, 1);
                break;
            }
            if (!isLetter(currentChar)) {
                // If it is a delimiter, add the previous word to the dictionary and set lastDelimiter
                lastDelimiter = mapTask.getOffset() + bytesRead;
                if (currWordSize == 0)
                    continue;
                else {
                    if (longestWord == null)
                        longestWord = currentWord.toString();
                    else if (longestWord.length() < currentWord.length())
                        longestWord = currentWord.toString();
                    if (dictionary.containsKey(currWordSize))
                        dictionary.put(currWordSize, dictionary.get(currWordSize) + 1);
                    else
                        dictionary.put(currWordSize, 1);
                }
                currentWord = new StringBuilder();
                currWordSize = 0;
            } else {
                // If it is a letter, add it to the currentWord and increase the currentWordSize
                currWordSize++;
                currentWord.append(currentChar);
            }

        }
        file.close();
        return new MapResult(mapTask.getDocumentName(), dictionary, longestWord);
    }

    public static boolean isLetter(char c) {
        return letters.indexOf(c) != -1;
    }
}
