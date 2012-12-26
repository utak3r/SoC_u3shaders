@"D:\devel\Microsoft DirectX SDK\Utilities\bin\x86\fxc" /nologo /Tps_2_0 /Emain /Zpr /Fctest\p20_%1 %1
@"D:\devel\Microsoft DirectX SDK\Utilities\bin\x86\fxc" /nologo /Tps_2_b /Emain /Zpr /Fctest\p2b_%1 %1
@"D:\devel\Microsoft DirectX SDK\Utilities\bin\x86\fxc" /DUSE_SHADER3=1 /nologo /Tps_3_0 /Emain /Zpr /Fctest\p30_%1 %1
@"C:\Program Files (x86)\NVIDIA Corporation\NVIDIA NVShaderPerf\nvshaderperf" -v 1 -o test\p2B_%1.log -a NV40 test\p2B_%1
@"C:\Program Files (x86)\NVIDIA Corporation\NVIDIA NVShaderPerf\nvshaderperf" -v 1 -o test\p30_%1.log -a NV40 test\p30_%1
@"C:\Program Files (x86)\NVIDIA Corporation\NVIDIA NVShaderPerf\nvshaderperf" -a NV40 test\p2B_%1 >> test\p2B_%1.log
@"C:\Program Files (x86)\NVIDIA Corporation\NVIDIA NVShaderPerf\nvshaderperf" -a NV40 test\p30_%1 >> test\p30_%1.log 
@"C:\Program Files (x86)\NVIDIA Corporation\NVIDIA NVShaderPerf\nvshaderperf" -a G70 test\p2B_%1 >> test\p2B_%1.log
@"C:\Program Files (x86)\NVIDIA Corporation\NVIDIA NVShaderPerf\nvshaderperf" -a G70 test\p30_%1 >> test\p30_%1.log 