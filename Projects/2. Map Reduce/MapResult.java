import java.util.Map;

public class MapResult {
    String documentName;
    Map<Integer, Integer> dictionary;
    String longestWord;

    public MapResult(String documentName, Map<Integer, Integer> dictionary, String longestWord) {
        this.documentName = documentName;
        this.dictionary = dictionary;
        this.longestWord = longestWord;
    }

    public String getDocumentName() {
        return documentName;
    }
}
