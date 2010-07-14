using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Xml;

using Gibbed.SaintsRow2.FileFormats;

namespace PegTool
{
    public static class XmlParser
    {
        public static PegFile ParseFile(string descFilePath)
        {
            XmlDocument document = new XmlDocument();
            document.Load(descFilePath);
            PegFile pegFile = new PegFile(false);
            XmlNode rootNode = null;
            foreach (XmlNode childNode in document.ChildNodes)
            {
                if (childNode.Name == "PegDescription")
                {
                    rootNode = childNode;
                    break;
                }
            }
            if (rootNode == null)
            {
                Console.WriteLine("This is not a valid peg_desc file: {0}", descFilePath);
                Console.WriteLine("Unable to find PegDescription node.", document.ChildNodes[1].Name);
                return null;
            }
            foreach (XmlNode node in rootNode.ChildNodes)
            {
                switch (node.Name)
                {
                    case "BigEndian":
                        bool.TryParse(node.InnerText, out pegFile.BigEndian);
                        break;
                    case "Magic":
                        if (node.InnerText != "GEKV")
                        {
                            Console.WriteLine("This is not a valid peg_desc file: {0}", descFilePath);
                            Console.WriteLine("Bad magic value. Got: {0}, expected: GEKV", node.InnerText);
                            return null;
                        }
                        break;
                    case "Version":
                        int version = 0;
                        int.TryParse(node.InnerText, out version);
                        if (version != 10)
                        {
                            Console.WriteLine("This is not a valid peg_desc file: {0}", descFilePath);
                            Console.WriteLine("Bad version value. Got: {0}, expected: 10", node.InnerText);
                            return null;
                        }
                        break;
                    case "Unknown06":
                        ushort.TryParse(node.InnerText, out pegFile.Unknown06);
                        break;
                    case "Unknown12":
                        ushort.TryParse(node.InnerText, out pegFile.Unknown12);
                        break;
                    case "Unknown16":
                        ushort.TryParse(node.InnerText, out pegFile.Unknown16);
                        break;
                    case "Entries":
                        ParseEntries(node, pegFile);
                        break;
                    //default:
                        //throw new Exception(String.Format("Encountered unknown XML element: {0}", node.Name));
                }
            }

            return pegFile;
        }

        public static void ParseEntries(XmlNode entriesNode, PegFile pegFile)
        {
            foreach (XmlNode entryNode in entriesNode.ChildNodes)
            {
                switch (entryNode.Name)
                {
                    case "Entry":
                        PegEntry entry = new PegEntry();
                        foreach (XmlNode node in entryNode.ChildNodes)
                        {
                            switch (node.Name)
                            {
                                case "Name":
                                    entry.Name = node.InnerText;
                                    break;
                                case "Frames":
                                    ParseFrames(node, entry);
                                    break;
                                //default:
                                    //throw new Exception(String.Format("Encountered unknown XML element: {0}", node.Name));
                            }
                        }
                        pegFile.Entries.Add(entry);
                        break;
                    //default:
                        //throw new Exception(String.Format("Encountered unknown XML element: {0}", entryNode.Name));
                }
            }
        }

        public static void ParseFrames(XmlNode framesNode, PegEntry entry)
        {
            foreach (XmlNode frameNode in framesNode.ChildNodes)
            {
                switch (frameNode.Name)
                {
                    case "Frame":
                        PegFrame frame = new PegFrame();
                        foreach (XmlNode node in frameNode.ChildNodes)
                        {
                            switch (node.Name)
                            {
                                case "Width":
                                    ushort.TryParse(node.InnerText, out frame.Width);
                                    break;
                                case "Height":
                                    ushort.TryParse(node.InnerText, out frame.Height);
                                    break;
                                case "Format":
                                    PegFormat format = (PegFormat)Enum.Parse(typeof(PegFormat), node.InnerText, true);
                                    frame.Format = (ushort)format;
                                    break;
                                case "Unknown0A":
                                    ushort.TryParse(node.InnerText, out frame.Unknown0A);
                                    break;
                                case "Unknown0C":
                                    uint.TryParse(node.InnerText, out frame.Unknown0C);
                                    break;
                                case "Unknown12":
                                    uint.TryParse(node.InnerText, out frame.Unknown12);
                                    break;
                                case "Unknown16":
                                    uint.TryParse(node.InnerText, out frame.Unknown16);
                                    break;
                                case "UnknownFlags1A":
                                    ushort.TryParse(node.InnerText, System.Globalization.NumberStyles.HexNumber, System.Globalization.CultureInfo.InvariantCulture, out frame.UnknownFlags1A);
                                    break;
                                case "Size":
                                    uint.TryParse(node.InnerText, out frame.Size);
                                    break;
                                case "Unknown20":
                                    uint.TryParse(node.InnerText, out frame.Unknown20);
                                    break;
                                case "Unknown24":
                                    uint.TryParse(node.InnerText, out frame.Unknown24);
                                    break;
                                case "Unknown28":
                                    uint.TryParse(node.InnerText, out frame.Unknown28);
                                    break;
                                case "Unknown2C":
                                    uint.TryParse(node.InnerText, out frame.Unknown2C);
                                    break;
                                //default:
                                    //throw new Exception(String.Format("Encountered unknown XML element: {0}", node.Name));
                            }
                        }
                        entry.Frames.Add(frame);
                        break;
                    default:
                        throw new Exception(String.Format("Encountered unknown XML element: {0}", frameNode.Name));
                }
            }
        }
    }
}
