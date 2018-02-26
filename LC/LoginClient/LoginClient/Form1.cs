using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace LoginClient
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void btnLogin_Click(object sender, EventArgs e)
        {
            String username = tbUsr.Text;
            String password = tbPswd.Text;
            EnvyUser user = new EnvyUser(username, password);
            String host = tbHost.Text;
            EnvyConnection conn = new EnvyConnection(host, 2356);
            if(conn.Connect())
            {
                Console.WriteLine("CONNECTED");
                var ap = new EnvyClientAuthProtocol(conn);
                ap.AuthenticateUser(user);
            }
        }
    }
}
