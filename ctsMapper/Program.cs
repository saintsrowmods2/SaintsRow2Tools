using System;
using System.Collections.Generic;
using System.Drawing;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using System.Text.RegularExpressions;

namespace ctsMapper
{
    static class Program
    {
        static void DrawCross(Graphics g, Pen p, float x, float y, string name)
        {
            x = x * -1f;
            //y = y * -1f;
            int xint = (int)((x / 5800) * 2048) + 1256;
            int yint = (int)((y / 5700) * 2048) + 1132;

            g.DrawLine(p, xint - 4, yint - 4, xint + 4f, yint + 4f);
            g.DrawLine(p, xint - 4, yint + 4, xint + 4f, yint - 4f);
            g.DrawString(name, new Font("Verdana", 12f, FontStyle.Regular), p.Brush, x, y);

            Console.WriteLine("Translated {0}: ({1}, {2})", name, x, y);
        }

        static void Main(string[] args)
        {
            string path = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            Image i = Image.FromFile(Path.Combine(path, "ui_map_world_city.tga_0.png"));

            Regex regex = new Regex("\\$Navpoint:.*?\"(.*?)\".*?\\$Pos:.*?<(.*?) (.*?) (.*?)>", RegexOptions.Compiled | RegexOptions.Singleline);

            string cts = File.ReadAllText(args[0]);

            MatchCollection matches = regex.Matches(cts);
            Console.WriteLine("Found {0} co-ords.", matches.Count);

            Pen p = new Pen(Brushes.White, 2f);
            Graphics g = Graphics.FromImage(i);

            DrawCross(g, new Pen(Brushes.Red, 2f), 0f, 0f, "(0, 0, 0)");

            foreach (Match m in matches)
            {
                float x, y, z;
                string name = m.Groups[1].Value.Trim();
                //if (!name.EndsWith("crib") && !name.EndsWith("icon") && !name.EndsWith("SHQ") && !name.EndsWith("Dock") && !name.EndsWith("Hangar"))
                    //continue;
                float.TryParse(m.Groups[2].Value, out x);
                float.TryParse(m.Groups[3].Value, out z);
                float.TryParse(m.Groups[4].Value, out y);

                Console.WriteLine("{0}: ({1}, {2}, {3})", name, x, y, z);

                DrawCross(g, p, x, y, name);
            }

            i.Save(Path.ChangeExtension(Path.GetFileName(args[0]), ".png"), System.Drawing.Imaging.ImageFormat.Png);

            Console.WriteLine("Done!");

            Console.ReadLine();
        }
    }
}
