@"D:\devel\Microsoft DirectX SDK\Utilities\bin\x86\fxc" /DUSE_SHADER3=1 /nologo /Gfp /Tps_3_0 /Emain /Zpr /Fctest\p30_%1 %1
@"C:\Program Files (x86)\NVIDIA Corporation\NVIDIA NVShaderPerf\nvshaderperf" -v 1 -o test\p30_%1.log -a NV40 test\p30_%1
@"C:\Program Files (x86)\NVIDIA Corporation\NVIDIA NVShaderPerf\nvshaderperf" -a NV40 test\p30_%1 >> test\p30_%1.log 
@"C:\Program Files (x86)\NVIDIA Corporation\NVIDIA NVShaderPerf\nvshaderperf" -minbranch -a NV40 test\p30_%1 >> test\p30_%1.log 
@"C:\Program Files (x86)\NVIDIA Corporation\NVIDIA NVShaderPerf\nvshaderperf" -a G70 test\p30_%1 >> test\p30_%1.log 
@"C:\Program Files (x86)\NVIDIA Corporation\NVIDIA NVShaderPerf\nvshaderperf" -minbranch -a G70 test\p30_%1 >> test\p30_%1.log 