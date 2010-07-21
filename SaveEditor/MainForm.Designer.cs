namespace SaveEditor
{
    partial class MainForm
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
            this.OpenSaveFile = new System.Windows.Forms.OpenFileDialog();
            this.MoneyUpDown = new System.Windows.Forms.NumericUpDown();
            this.label1 = new System.Windows.Forms.Label();
            this.SaveChanges = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.PistolAmmoUpDown = new System.Windows.Forms.NumericUpDown();
            this.PistolClipUpDown = new System.Windows.Forms.NumericUpDown();
            this.LoadFromFile = new System.Windows.Forms.Button();
            this.SMGClipUpDown = new System.Windows.Forms.NumericUpDown();
            this.SMGAmmoUpDown = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.ShotgunClipUpDown = new System.Windows.Forms.NumericUpDown();
            this.ShotgunAmmoUpDown = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.RifleClipUpDown = new System.Windows.Forms.NumericUpDown();
            this.RifleAmmoUpDown = new System.Windows.Forms.NumericUpDown();
            this.label5 = new System.Windows.Forms.Label();
            this.SpecialClipUpDown = new System.Windows.Forms.NumericUpDown();
            this.SpecialAmmoUpDown = new System.Windows.Forms.NumericUpDown();
            this.label6 = new System.Windows.Forms.Label();
            this.ThrownClipUpDown = new System.Windows.Forms.NumericUpDown();
            this.ThrownAmmoUpDown = new System.Windows.Forms.NumericUpDown();
            this.label7 = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.MoneyUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.PistolAmmoUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.PistolClipUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.SMGClipUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.SMGAmmoUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ShotgunClipUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ShotgunAmmoUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RifleClipUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.RifleAmmoUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.SpecialClipUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.SpecialAmmoUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ThrownClipUpDown)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.ThrownAmmoUpDown)).BeginInit();
            this.SuspendLayout();
            // 
            // OpenSaveFile
            // 
            this.OpenSaveFile.FileName = "*.bbsave";
            this.OpenSaveFile.Filter = "Saints Row 2 save|*.bbsave|All files|*.*";
            // 
            // MoneyUpDown
            // 
            this.MoneyUpDown.DecimalPlaces = 2;
            this.MoneyUpDown.Location = new System.Drawing.Point(207, 7);
            this.MoneyUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            131072});
            this.MoneyUpDown.Name = "MoneyUpDown";
            this.MoneyUpDown.Size = new System.Drawing.Size(120, 20);
            this.MoneyUpDown.TabIndex = 5;
            this.MoneyUpDown.ThousandsSeparator = true;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(12, 9);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(42, 13);
            this.label1.TabIndex = 4;
            this.label1.Text = "Money:";
            // 
            // SaveChanges
            // 
            this.SaveChanges.Location = new System.Drawing.Point(252, 285);
            this.SaveChanges.Name = "SaveChanges";
            this.SaveChanges.Size = new System.Drawing.Size(75, 23);
            this.SaveChanges.TabIndex = 3;
            this.SaveChanges.Text = "Save";
            this.SaveChanges.UseVisualStyleBackColor = true;
            this.SaveChanges.Click += new System.EventHandler(this.SaveChanges_Click);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(12, 35);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(35, 13);
            this.label2.TabIndex = 6;
            this.label2.Text = "Pistol:";
            // 
            // PistolAmmoUpDown
            // 
            this.PistolAmmoUpDown.Location = new System.Drawing.Point(207, 33);
            this.PistolAmmoUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.PistolAmmoUpDown.Name = "PistolAmmoUpDown";
            this.PistolAmmoUpDown.Size = new System.Drawing.Size(120, 20);
            this.PistolAmmoUpDown.TabIndex = 7;
            // 
            // PistolClipUpDown
            // 
            this.PistolClipUpDown.Location = new System.Drawing.Point(81, 33);
            this.PistolClipUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.PistolClipUpDown.Name = "PistolClipUpDown";
            this.PistolClipUpDown.Size = new System.Drawing.Size(120, 20);
            this.PistolClipUpDown.TabIndex = 8;
            // 
            // LoadFromFile
            // 
            this.LoadFromFile.Location = new System.Drawing.Point(171, 285);
            this.LoadFromFile.Name = "LoadFromFile";
            this.LoadFromFile.Size = new System.Drawing.Size(75, 23);
            this.LoadFromFile.TabIndex = 9;
            this.LoadFromFile.Text = "Load";
            this.LoadFromFile.UseVisualStyleBackColor = true;
            this.LoadFromFile.Click += new System.EventHandler(this.LoadFromFile_Click);
            // 
            // SMGClipUpDown
            // 
            this.SMGClipUpDown.Location = new System.Drawing.Point(81, 59);
            this.SMGClipUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.SMGClipUpDown.Name = "SMGClipUpDown";
            this.SMGClipUpDown.Size = new System.Drawing.Size(120, 20);
            this.SMGClipUpDown.TabIndex = 12;
            // 
            // SMGAmmoUpDown
            // 
            this.SMGAmmoUpDown.Location = new System.Drawing.Point(207, 59);
            this.SMGAmmoUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.SMGAmmoUpDown.Name = "SMGAmmoUpDown";
            this.SMGAmmoUpDown.Size = new System.Drawing.Size(120, 20);
            this.SMGAmmoUpDown.TabIndex = 11;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(12, 61);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(34, 13);
            this.label3.TabIndex = 10;
            this.label3.Text = "SMG:";
            // 
            // ShotgunClipUpDown
            // 
            this.ShotgunClipUpDown.Location = new System.Drawing.Point(81, 85);
            this.ShotgunClipUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.ShotgunClipUpDown.Name = "ShotgunClipUpDown";
            this.ShotgunClipUpDown.Size = new System.Drawing.Size(120, 20);
            this.ShotgunClipUpDown.TabIndex = 15;
            // 
            // ShotgunAmmoUpDown
            // 
            this.ShotgunAmmoUpDown.Location = new System.Drawing.Point(207, 85);
            this.ShotgunAmmoUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.ShotgunAmmoUpDown.Name = "ShotgunAmmoUpDown";
            this.ShotgunAmmoUpDown.Size = new System.Drawing.Size(120, 20);
            this.ShotgunAmmoUpDown.TabIndex = 14;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(12, 87);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(50, 13);
            this.label4.TabIndex = 13;
            this.label4.Text = "Shotgun:";
            // 
            // RifleClipUpDown
            // 
            this.RifleClipUpDown.Location = new System.Drawing.Point(81, 111);
            this.RifleClipUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.RifleClipUpDown.Name = "RifleClipUpDown";
            this.RifleClipUpDown.Size = new System.Drawing.Size(120, 20);
            this.RifleClipUpDown.TabIndex = 18;
            // 
            // RifleAmmoUpDown
            // 
            this.RifleAmmoUpDown.Location = new System.Drawing.Point(207, 111);
            this.RifleAmmoUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.RifleAmmoUpDown.Name = "RifleAmmoUpDown";
            this.RifleAmmoUpDown.Size = new System.Drawing.Size(120, 20);
            this.RifleAmmoUpDown.TabIndex = 17;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(12, 113);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(31, 13);
            this.label5.TabIndex = 16;
            this.label5.Text = "Rifle:";
            // 
            // SpecialClipUpDown
            // 
            this.SpecialClipUpDown.Location = new System.Drawing.Point(81, 137);
            this.SpecialClipUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.SpecialClipUpDown.Name = "SpecialClipUpDown";
            this.SpecialClipUpDown.Size = new System.Drawing.Size(120, 20);
            this.SpecialClipUpDown.TabIndex = 21;
            // 
            // SpecialAmmoUpDown
            // 
            this.SpecialAmmoUpDown.Location = new System.Drawing.Point(207, 137);
            this.SpecialAmmoUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.SpecialAmmoUpDown.Name = "SpecialAmmoUpDown";
            this.SpecialAmmoUpDown.Size = new System.Drawing.Size(120, 20);
            this.SpecialAmmoUpDown.TabIndex = 20;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(12, 139);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(45, 13);
            this.label6.TabIndex = 19;
            this.label6.Text = "Special:";
            // 
            // ThrownClipUpDown
            // 
            this.ThrownClipUpDown.Location = new System.Drawing.Point(81, 163);
            this.ThrownClipUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.ThrownClipUpDown.Name = "ThrownClipUpDown";
            this.ThrownClipUpDown.Size = new System.Drawing.Size(120, 20);
            this.ThrownClipUpDown.TabIndex = 24;
            // 
            // ThrownAmmoUpDown
            // 
            this.ThrownAmmoUpDown.Location = new System.Drawing.Point(207, 163);
            this.ThrownAmmoUpDown.Maximum = new decimal(new int[] {
            2147483647,
            0,
            0,
            0});
            this.ThrownAmmoUpDown.Name = "ThrownAmmoUpDown";
            this.ThrownAmmoUpDown.Size = new System.Drawing.Size(120, 20);
            this.ThrownAmmoUpDown.TabIndex = 23;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(12, 165);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(46, 13);
            this.label7.TabIndex = 22;
            this.label7.Text = "Thrown:";
            // 
            // MainForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(339, 320);
            this.Controls.Add(this.ThrownClipUpDown);
            this.Controls.Add(this.ThrownAmmoUpDown);
            this.Controls.Add(this.label7);
            this.Controls.Add(this.SpecialClipUpDown);
            this.Controls.Add(this.SpecialAmmoUpDown);
            this.Controls.Add(this.label6);
            this.Controls.Add(this.RifleClipUpDown);
            this.Controls.Add(this.RifleAmmoUpDown);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.ShotgunClipUpDown);
            this.Controls.Add(this.ShotgunAmmoUpDown);
            this.Controls.Add(this.label4);
            this.Controls.Add(this.SMGClipUpDown);
            this.Controls.Add(this.SMGAmmoUpDown);
            this.Controls.Add(this.label3);
            this.Controls.Add(this.LoadFromFile);
            this.Controls.Add(this.PistolClipUpDown);
            this.Controls.Add(this.PistolAmmoUpDown);
            this.Controls.Add(this.label2);
            this.Controls.Add(this.MoneyUpDown);
            this.Controls.Add(this.label1);
            this.Controls.Add(this.SaveChanges);
            this.Name = "MainForm";
            this.Text = "Save Editor";
            this.Shown += new System.EventHandler(this.MainForm_Shown);
            ((System.ComponentModel.ISupportInitialize)(this.MoneyUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.PistolAmmoUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.PistolClipUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.SMGClipUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.SMGAmmoUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ShotgunClipUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ShotgunAmmoUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RifleClipUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.RifleAmmoUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.SpecialClipUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.SpecialAmmoUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ThrownClipUpDown)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.ThrownAmmoUpDown)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.OpenFileDialog OpenSaveFile;
        private System.Windows.Forms.NumericUpDown MoneyUpDown;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.Button SaveChanges;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.NumericUpDown PistolAmmoUpDown;
        private System.Windows.Forms.NumericUpDown PistolClipUpDown;
        private System.Windows.Forms.Button LoadFromFile;
        private System.Windows.Forms.NumericUpDown SMGClipUpDown;
        private System.Windows.Forms.NumericUpDown SMGAmmoUpDown;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.NumericUpDown ShotgunClipUpDown;
        private System.Windows.Forms.NumericUpDown ShotgunAmmoUpDown;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.NumericUpDown RifleClipUpDown;
        private System.Windows.Forms.NumericUpDown RifleAmmoUpDown;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.NumericUpDown SpecialClipUpDown;
        private System.Windows.Forms.NumericUpDown SpecialAmmoUpDown;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.NumericUpDown ThrownClipUpDown;
        private System.Windows.Forms.NumericUpDown ThrownAmmoUpDown;
        private System.Windows.Forms.Label label7;
    }
}

