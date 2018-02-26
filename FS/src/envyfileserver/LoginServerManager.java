package envyfileserver;

import javax.swing.JOptionPane;
import envyfileserver.login.loginDialog;
import envyfileserver.login.DBManager;
import envyfileserver.net.EnvyFTPServer;
import java.net.URL;
import java.net.URLClassLoader;
import java.util.logging.Level;
import java.util.logging.Logger;

public class LoginServerManager extends LoginServerCore {

    static final Logger log = Logger.getLogger("envyfileserver");

    private EnvyFTPServer server;

    public static void main(String[] args) {
        try {
            Class.forName("org.sqlite.JDBC");
            new LoginServerManager().run();
        } catch (ClassNotFoundException ex) {
            Logger.getLogger(LoginServerManager.class.getName()).log(Level.SEVERE, null, ex);
        }
    }

    public void init() {
        //Set up our server sockets and what not, but for now just initialize database
        super.init();
        new EnvyFTPServer().start();
    }

}
