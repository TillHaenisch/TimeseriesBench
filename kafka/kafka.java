import kafka.javaapi.producer.Producer;
import org.apache.kafka.clients.producer.KafkaProducer;
import org.apache.kafka.clients.producer.ProducerRecord;
import kafka.producer.ProducerConfig;
import java.util.*;
import java.io.*;


public class kafka {
	
	private static byte[] convertToBytes(Object object) throws IOException {
	    try (ByteArrayOutputStream bos = new ByteArrayOutputStream();
	         ObjectOutput out = new ObjectOutputStream(bos)) {
	        out.writeObject(object);
	        return bos.toByteArray();
	    } 
	}
	
	public static void main(String [] args) throws Exception {
		
		Properties props = new Properties();
 
		props.put("bootstrap.servers", "localhost:9092");
		props.put("key.serializer", "org.apache.kafka.common.serialization.StringSerializer");
		props.put("value.serializer", "org.apache.kafka.common.serialization.ByteArraySerializer");
 			
		final int iValues = 100000;
		final int iSensors = 10;
		final int VALUE_TYPE_TEMP_CENTIGRADE = 42;
		
		String topicName = "test";
		
    KafkaProducer<byte[], byte[]> producer = new KafkaProducer<byte[],byte[]>(props);
     
		
		 Measurement m = new Measurement();
		 
  	for(int i=0;i<iValues;i++) {
  		long t = System.currentTimeMillis();
			for(int j=0;j<iSensors;j++) {
				m.value=i+j;
				m.time_ms = t;
				m.sensor_id = j;
		
				byte[] payload = convertToBytes(m);
	      ProducerRecord<byte[],byte[]> record = new ProducerRecord<byte[],byte[]>(topicName, payload);
 
				producer.send(record);
			}
		}
		producer.close();
}
}