using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Drawing.Imaging;
using System.IO;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace ILIScreenshotViewer
{
    public partial class Form1 : Form
    {
        public Form1()
        {
            InitializeComponent();
        }

        private void btnShow_Click(object sender, EventArgs e)
        {
            lblFailed.Visible = false;
            var bitmap = new Bitmap(320, 240, PixelFormat.Format24bppRgb);

            //var hexString = Clipboard.GetText();
            //for (int x = 0; x < 320; x++)
            //{
            //    for (int y = 0; y < 240; y++)
            //    {
            //        var pixelColorString = hexString.Substring(4*x + 4*320*y, 4);

            //        var pix = Convert.ToUInt16(pixelColorString, 16);


            //        //* For PixelFormat.Format16bppRgb565
            //        //var r = BitConverter.GetBytes((UInt16)((pix & 0xF800) >> 11));

            //        int red = ((pix >> 11)*527 + 23) >> 6;
            //        int green = (((pix >> 5) & 0x003F)*259 + 33) >> 6;
            //        int blue = ((pix & 0x001F)*527 + 23) >> 6;

            //        bitmap.SetPixel(x, y, Color.FromArgb(red, green, blue));
            //    }
            //}


            try
            {
                string hexString = Clipboard.GetText();
                //for (int x = 0; x < 320; x++)
                //{
                //    for (int y = 0; y < 240; y++)
                //    {
                //        var pixelColorString = hexString.Substring(6 * (x + 320 * y), 6);

                //        var r = Convert.ToByte(pixelColorString.Substring(0, 2), 16);
                //        var g = Convert.ToByte(pixelColorString.Substring(2, 2), 16);
                //        var b = Convert.ToByte(pixelColorString.Substring(4, 2), 16);

                //        //* For PixelFormat.Format16bppRgb565
                //        //var r = BitConverter.GetBytes((UInt16)((pix & 0xF800) >> 11));

                //        //int red = ((pix >> 11) * 527 + 23) >> 6;
                //        //int green = (((pix >> 5) & 0x003F) * 259 + 33) >> 6;
                //        //int blue = ((pix & 0x001F) * 527 + 23) >> 6;

                //        bitmap.SetPixel(x, y, Color.FromArgb(r, g, b));
                //    }
                //}
                int x = 0, y = 0;
                var totalImageDataSize = Convert.ToUInt32(hexString.Substring(hexString.Length - 8, 8), 16);
                if (hexString.Length - 8 == totalImageDataSize)
                {
                    for (int i = 0; i < totalImageDataSize; i += 10) // 6 bytes for color, 4 bytes for length
                    {
                        var pixelColorString = hexString.Substring(i, 6);
                        var r = Convert.ToByte(pixelColorString.Substring(0, 2), 16);
                        var g = Convert.ToByte(pixelColorString.Substring(2, 2), 16);
                        var b = Convert.ToByte(pixelColorString.Substring(4, 2), 16);

                        var pixelCountString = hexString.Substring(i + 6, 4);
                        var pixelCount = Convert.ToUInt16(pixelCountString, 16);

                        for (int p = 0; p < pixelCount; p++)
                        {
                            bitmap.SetPixel(x, y, Color.FromArgb(r, g, b));
                            x++;
                            if (x > 319)
                            {
                                x = 0;
                                y++;
                            }
                        }
                    }

                    pictureBox.Image = bitmap;
                    btnSaveAs.Visible = true;
                }
                else
                {
                    lblFailed.Text = "Image Data length mismatch.";
                    pictureBox.Image = null;
                    lblFailed.Visible = true;
                    btnSaveAs.Visible = false;
                }
            }
            catch
            {
                lblFailed.Text = "Failed to load an image from clipboard\nTry increasing ILI9341_SPI_CLKDIVIDER or\nchange to NORMAL or EXTENDED SPI mode.";
                pictureBox.Image = null;
                lblFailed.Visible = true;
                btnSaveAs.Visible = false;
            }
        }

        private void btnSaveAs_Click(object sender, EventArgs e)
        {
            saveFileDialog.FileName = Path.GetFileNameWithoutExtension(saveFileDialog.FileName);
            var result = saveFileDialog.ShowDialog();
            if (result == DialogResult.OK)
            {
                ImageFormat imageFormat = ImageFormat.Bmp;
                switch (Path.GetExtension(saveFileDialog.FileName).ToLower())
                {
                    case ".jpg":
                        imageFormat = ImageFormat.Jpeg;
                        break;
                    case ".png":
                        imageFormat = ImageFormat.Png;
                        break;
                    case ".gif":
                        imageFormat = ImageFormat.Gif;
                        break;
                }
                pictureBox.Image.Save(saveFileDialog.FileName, imageFormat);
            }

    }

    }
}
