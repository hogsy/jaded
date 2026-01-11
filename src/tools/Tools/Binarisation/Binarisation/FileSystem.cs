using System;
using System.IO;

namespace Binarisation
{
	/// <summary>
	/// Summary description for FileSystem.
	/// </summary>
	public class FileSystem
	{
		public FileSystem()
		{
			//
			// TODO: Add constructor logic here
			//
		}

        public static void CreateDirectories(string _sDir, bool _bSetCurrent)
        {			
            if(!Directory.Exists(_sDir)) 
            {
                Directory.CreateDirectory(_sDir);
            }
            if (_bSetCurrent)
                Directory.SetCurrentDirectory(_sDir);
        }

		// Copy directory structure recursively
		public static void CopyDirectory(string Src,string Dst)
		{
			String[] Files;

			if(Dst[Dst.Length-1]!=Path.DirectorySeparatorChar) 
				Dst+=Path.DirectorySeparatorChar;
			if(!Directory.Exists(Dst)) Directory.CreateDirectory(Dst);
			Files=Directory.GetFileSystemEntries(Src);
			foreach(string Element in Files)
			{
				// Sub directories
				if(Directory.Exists(Element)) 
					CopyDirectory(Element,Dst+Path.GetFileName(Element));
					// Files in directory
				else 
				{
                    CopyFile(Element, Dst+Path.GetFileName(Element));
				}
			}
		}

        public static void CopyFile(string src, string dst)
        {
            if (File.Exists(dst))
                File.SetAttributes(dst,FileAttributes.Normal);
            
			if (dst.EndsWith(@"\"))			
				dst += Path.GetFileName(src);

            File.Copy(src, dst, true);
        }

		public static void DeleteAllDirectory(string _sSrc)
		{
			DeleteAllFilesInDirectory(_sSrc);

			Directory.Delete(_sSrc,true);
		}

		public static void DeleteAllFilesInDirectory(string _sSrc)
		{
            if (Directory.Exists(_sSrc))
            {            
                String[] Files;

                Files = Directory.GetFileSystemEntries(_sSrc);
    		
                foreach(string Element in Files)
                {
                    if (Directory.Exists(Element))
                        DeleteAllDirectory(Element);
                    else
                    {
                        File.SetAttributes(Element,System.IO.FileAttributes.Normal);				
                        File.Delete(Element);
                    }
                }
            }
		}		

		public static bool DeleteIfExist(string Src)
		{
			if (File.Exists(Src))
			{
				File.Delete(Src);				
			}
			else 
			{
				return false;
			}
			return true;
		}

		public static bool CopyIfExist(string Src, string Dest)
		{
			if (File.Exists(Src))
			{
				if (File.Exists(Dest))
				{
					File.SetAttributes(Dest,FileAttributes.Normal);
				}
				CopyFile(Src,Dest);				
			}
			else 
			{
				return false;
			}
			return true;
		}
	}
}
