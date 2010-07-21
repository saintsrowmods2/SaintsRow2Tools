using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;

using Gibbed.SaintsRow2.Helpers;

namespace SaveEditor
{
    public partial class MainForm : Form
    {
        private string ActiveFile;
        public MainForm()
        {
            InitializeComponent();
        }

        private void LoadFile(string file)
        {
            FileStream fs = new FileStream(file, FileMode.Open, FileAccess.Read, FileShare.Read);

            fs.Seek(0x00B4, SeekOrigin.Begin);
            MoneyUpDown.Value = (decimal)fs.ReadS32()/100;

            fs.Seek(0x00F4, SeekOrigin.Begin);
            PistolClipUpDown.Value = fs.ReadS32();
            PistolAmmoUpDown.Value = fs.ReadS32();

            fs.Seek(0x0110, SeekOrigin.Begin);
            SMGClipUpDown.Value = fs.ReadS32();
            SMGAmmoUpDown.Value = fs.ReadS32();

            fs.Seek(0x012C, SeekOrigin.Begin);
            ShotgunClipUpDown.Value = fs.ReadS32();
            ShotgunAmmoUpDown.Value = fs.ReadS32();

            fs.Seek(0x0148, SeekOrigin.Begin);
            RifleClipUpDown.Value = fs.ReadS32();
            RifleAmmoUpDown.Value = fs.ReadS32();

            fs.Seek(0x164, SeekOrigin.Begin);
            SpecialClipUpDown.Value = fs.ReadS32();
            SpecialAmmoUpDown.Value = fs.ReadS32();

            fs.Seek(0x180, SeekOrigin.Begin);
            ThrownClipUpDown.Value = fs.ReadS32();
            ThrownAmmoUpDown.Value = fs.ReadS32();

            fs.Close();
        }

        private void SaveFile(string file)
        {
            FileStream fs = new FileStream(file, FileMode.Open, FileAccess.ReadWrite, FileShare.Read);

            fs.Seek(0x00B4, SeekOrigin.Begin);
            fs.WriteS32((int)(MoneyUpDown.Value * 100));

            fs.Seek(0x00F4, SeekOrigin.Begin);
            fs.WriteS32((int)PistolClipUpDown.Value);
            fs.WriteS32((int)PistolAmmoUpDown.Value);

            fs.Seek(0x0110, SeekOrigin.Begin);
            fs.WriteS32((int)SMGClipUpDown.Value);
            fs.WriteS32((int)SMGAmmoUpDown.Value);

            fs.Seek(0x012C, SeekOrigin.Begin);
            fs.WriteS32((int)ShotgunClipUpDown.Value);
            fs.WriteS32((int)ShotgunAmmoUpDown.Value);

            fs.Seek(0x0148, SeekOrigin.Begin);
            fs.WriteS32((int)RifleClipUpDown.Value);
            fs.WriteS32((int)RifleAmmoUpDown.Value);

            fs.Seek(0x164, SeekOrigin.Begin);
            fs.WriteS32((int)SpecialClipUpDown.Value);
            fs.WriteS32((int)SpecialAmmoUpDown.Value);

            fs.Seek(0x180, SeekOrigin.Begin);
            fs.WriteS32((int)ThrownClipUpDown.Value);
            fs.WriteS32((int)ThrownAmmoUpDown.Value);

            fs.Close();
        }

        private void MainForm_Shown(object sender, EventArgs e)
        {
            if (OpenSaveFile.ShowDialog() != DialogResult.OK)
            {
                Application.Exit();
            }
            else
            {
                ActiveFile = OpenSaveFile.FileName;
                LoadFile(ActiveFile);
                this.Show();
            }
        }

        private void SaveChanges_Click(object sender, EventArgs e)
        {
            SaveFile(ActiveFile);
        }

        private void LoadFromFile_Click(object sender, EventArgs e)
        {
            if (OpenSaveFile.ShowDialog() != DialogResult.OK)
            {
            }
            else
            {
                ActiveFile = OpenSaveFile.FileName;
                LoadFile(ActiveFile);
                this.Show();
            }
        }
    }
}
