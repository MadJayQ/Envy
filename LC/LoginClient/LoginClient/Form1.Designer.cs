namespace LoginClient
{
    partial class Form1
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.tbUsr = new System.Windows.Forms.TextBox();
            this.tbPswd = new System.Windows.Forms.TextBox();
            this.btnLogin = new System.Windows.Forms.Button();
            this.lblUsr = new System.Windows.Forms.Label();
            this.lblPswd = new System.Windows.Forms.Label();
            this.tbHost = new System.Windows.Forms.TextBox();
            this.SuspendLayout();
            // 
            // tbUsr
            // 
            this.tbUsr.Location = new System.Drawing.Point(45, 107);
            this.tbUsr.Name = "tbUsr";
            this.tbUsr.Size = new System.Drawing.Size(157, 22);
            this.tbUsr.TabIndex = 0;
            // 
            // tbPswd
            // 
            this.tbPswd.Location = new System.Drawing.Point(45, 186);
            this.tbPswd.Name = "tbPswd";
            this.tbPswd.Size = new System.Drawing.Size(157, 22);
            this.tbPswd.TabIndex = 1;
            this.tbPswd.UseSystemPasswordChar = true;
            // 
            // btnLogin
            // 
            this.btnLogin.Location = new System.Drawing.Point(45, 242);
            this.btnLogin.Name = "btnLogin";
            this.btnLogin.Size = new System.Drawing.Size(157, 23);
            this.btnLogin.TabIndex = 2;
            this.btnLogin.Text = "Login";
            this.btnLogin.UseVisualStyleBackColor = true;
            this.btnLogin.Click += new System.EventHandler(this.btnLogin_Click);
            // 
            // lblUsr
            // 
            this.lblUsr.AutoSize = true;
            this.lblUsr.Location = new System.Drawing.Point(84, 70);
            this.lblUsr.Name = "lblUsr";
            this.lblUsr.Size = new System.Drawing.Size(73, 17);
            this.lblUsr.TabIndex = 3;
            this.lblUsr.Text = "Username";
            // 
            // lblPswd
            // 
            this.lblPswd.AutoSize = true;
            this.lblPswd.Location = new System.Drawing.Point(84, 152);
            this.lblPswd.Name = "lblPswd";
            this.lblPswd.Size = new System.Drawing.Size(69, 17);
            this.lblPswd.TabIndex = 4;
            this.lblPswd.Text = "Password";
            // 
            // tbHost
            // 
            this.tbHost.Location = new System.Drawing.Point(74, 291);
            this.tbHost.Name = "tbHost";
            this.tbHost.Size = new System.Drawing.Size(92, 22);
            this.tbHost.TabIndex = 5;
            this.tbHost.Text = "127.0.0.1";
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 16F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(254, 348);
            this.Controls.Add(this.tbHost);
            this.Controls.Add(this.lblPswd);
            this.Controls.Add(this.lblUsr);
            this.Controls.Add(this.btnLogin);
            this.Controls.Add(this.tbPswd);
            this.Controls.Add(this.tbUsr);
            this.Name = "Form1";
            this.Text = "Form1";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox tbUsr;
        private System.Windows.Forms.TextBox tbPswd;
        private System.Windows.Forms.Button btnLogin;
        private System.Windows.Forms.Label lblUsr;
        private System.Windows.Forms.Label lblPswd;
        private System.Windows.Forms.TextBox tbHost;
    }
}

