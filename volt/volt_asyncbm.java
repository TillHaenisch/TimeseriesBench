import org.voltdb.*;
import org.voltdb.client.*;
import java.util.Random;

public class volt_asyncbm {

        public static void main(String[] args) throws Exception {
        org.voltdb.client.Client client;
        Random rand = new Random();
        client = ClientFactory.createClient();
		
		
        client.createConnection("127.0.0.1");
        final long zeilen;
        zeilen = 100000;
        final long startTime = System.currentTimeMillis();
        for ( int i = 0; i < zeilen; i++) {
				
			client.callProcedure(new NullCallback(),
                         "insert_sd",
                         Integer.toString(i), 1234, 0, i % 3, rand.nextInt(100000));
					 }
		final long endTime = System.currentTimeMillis();
		
		final double delta = (double)endTime - (double)startTime;

		System.out.printf("Runtime: %f.\n", delta);
		System.out.printf("Time per Insert: %8.8f\n", delta / zeilen);
		System.out.printf("Inserts per Second: %8.8f\n", 1 / (delta / zeilen)*1000);
		
        client.close();
    }
}
