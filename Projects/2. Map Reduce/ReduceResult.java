import java.io.File;

public class ReduceResult {
    String documentName;
    float rank;
    int longestWordSize;
    int longestWordNo;

    public ReduceResult(String documentName, float rank, int longestWordSize, int longestWordNo) {
        this.documentName = documentName;
        this.rank = rank;
        this.longestWordSize = longestWordSize;
        this.longestWordNo = longestWordNo;
    }

    @Override
    public String toString() {
        File f = new File(documentName);
        String document = f.getName();
        return document + "," + String.format("%.2f", rank) + "," + longestWordSize + "," + longestWordNo;
    }

    public float getRank() {
        return rank;
    }
}
