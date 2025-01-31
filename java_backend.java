import java.io.*;
import java.net.HttpURLConnection;
import java.net.URL;

public class FloodAlertSystem {
    
    private static final String BOT_TOKEN = "YOUR_TELEGRAM_BOT_TOKEN";
    private static final String CHAT_ID = "YOUR_CHAT_ID";

    public static void sendTelegramAlert(String message) {
        try {
            String urlString = "https://api.telegram.org/bot%s/sendMessage?chat_id=%s&text=%s";
            urlString = String.format(urlString, BOT_TOKEN, CHAT_ID, message);
            
            URL url = new URL(urlString);
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            conn.setRequestMethod("GET");
            
            BufferedReader in = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            String inputLine;
            StringBuffer response = new StringBuffer();
            
            while ((inputLine = in.readLine()) != null) {
                response.append(inputLine);
            }
            in.close();
            
            System.out.println("Alert sent successfully");
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    public static void main(String[] args) {
        // Example usage
        sendTelegramAlert("Test Alert from Java Server");
    }
}
