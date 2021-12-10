
namespace LRInstaller
{
    partial class InstallerForm
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
            this.Install_button = new System.Windows.Forms.Button();
            this.uninstall_button = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // Install_button
            // 
            this.Install_button.Location = new System.Drawing.Point(149, 169);
            this.Install_button.Name = "Install_button";
            this.Install_button.Size = new System.Drawing.Size(128, 58);
            this.Install_button.TabIndex = 0;
            this.Install_button.Text = "Install";
            this.Install_button.UseVisualStyleBackColor = true;
            this.Install_button.Click += new System.EventHandler(this.Install_button_Click);
            // 
            // uninstall_button
            // 
            this.uninstall_button.Location = new System.Drawing.Point(354, 169);
            this.uninstall_button.Name = "uninstall_button";
            this.uninstall_button.Size = new System.Drawing.Size(128, 58);
            this.uninstall_button.TabIndex = 0;
            this.uninstall_button.Text = "Uninstall";
            this.uninstall_button.UseVisualStyleBackColor = true;
            this.uninstall_button.Click += new System.EventHandler(this.uninstall_button_Click);
            // 
            // InstallerForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(8F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(800, 450);
            this.Controls.Add(this.uninstall_button);
            this.Controls.Add(this.Install_button);
            this.Name = "InstallerForm";
            this.Text = "InstallerForm";
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.Button Install_button;
        private System.Windows.Forms.Button uninstall_button;
    }
}