

import java.io.*;
import java.util.ArrayList;

public class OpenGLES_Shader_Exporter {
	public static void main(String[] args) {
        
        String os = System.getProperty("os.name");
        System.out.println("OS name: " + os);
        
        boolean isWindows = false;
        if (os.startsWith("Windows")) {
        	isWindows = true;
    	}

    	String splash = isWindows ? "\\" : "/";
    	String workingDir = System.getProperty("user.dir");
        	
        String srcDir = "";
		if (args.length < 1 || !(new File(args[0])).isDirectory()) {
        	System.out.println("Usage: OpenGLES_Shader_Exporter %Shader Dir% Now use default dir");
            srcDir = workingDir + splash + "shader";
		} else {
			srcDir = args[0];
		}
        
		if (!srcDir.endsWith(splash)){
			srcDir += splash;
		}
		File dir = new File(srcDir);
        
		ArrayList<String> vlist = new ArrayList<String>();
		ArrayList<String> plist = new ArrayList<String>();
		ArrayList<String> namelist = new ArrayList<String>();
		
		try {
			String shaders[] = dir.list();
			for (int i = 0; i < shaders.length; i++) {
				File aShader = new File(srcDir + shaders[i]);
				if (!aShader.isDirectory()){
					System.out.println("Warning! " + shaders[i] + " is not valid, ignored");
					continue;
				}
				
                String vshader = null;
                String pshader = null;
				String vpshaders[] = aShader.list();
                for (int j = 0;j < vpshaders.length;j++){
                    if (vpshaders[j].equals("vertex.sdr")){
                        vshader = vpshaders[j];
                    }
                    if (vpshaders[j].equals("fragment.sdr")){
                        pshader = vpshaders[j];
                    }
                }
				if (vshader == null || pshader == null){
					System.out.println("Warning! Shader " + aShader + " is not valid, ignored; Note that for each Shader there should be only two files: vertex.sdr and fragment.sdr");
					continue;
                }
				
				// a valid shader found add it
				vlist.add(aShader + "/" + vshader);
				plist.add(aShader + "/" + pshader);
				namelist.add(shaders[i]);
			}
			
			DataOutputStream out = new DataOutputStream(new FileOutputStream("glshaders.h"));
			try{
				out.writeBytes("#ifndef HOUYI_SHADERS\n");
				out.writeBytes("#define HOUYI_SHADERS\n\n");
				out.writeBytes("#define HOUYI_SHADER_COUNT " + vlist.size() + "\n\n");
				out.writeBytes("static const char* vshaders["+vlist.size()+"] = {\n");
				for (int i = 0;i < vlist.size();i++){
					BufferedReader in = new BufferedReader(new FileReader(
							vlist.get(i)));
	
					String line = in.readLine();
					while (line != null) {
						line = "\"" + line + "\\n\"\n";
						out.writeBytes(line);
						
						line = in.readLine();
					}
					
					if (i < vlist.size() - 1){
						out.writeBytes(", \n");
					}
					in.close();
				}
				out.writeBytes("};\n\n");
				
				out.writeBytes("static const char* pshaders["+plist.size()+"] = {\n");
				for (int i = 0;i < plist.size();i++){
					BufferedReader in = new BufferedReader(new FileReader(
							plist.get(i)));
	
					String line = in.readLine();
					while (line != null) {
						line = "\"" + line + "\\n\"\n";
						out.writeBytes(line);
						
						line = in.readLine();
					}
					
					if (i < vlist.size() - 1){
						out.writeBytes(", \n");
					}
					in.close();
				}
				out.writeBytes("};\n\n");
				
				out.writeBytes("#endif\n\n");
			} finally {
				out.close();
			}
			
            DataOutputStream cout = null;
            try{
                cout = new DataOutputStream(new FileOutputStream(
                                                                                     "HIShaders.h"));
                cout.writeBytes("// automatically generated interface\n");
                cout.writeBytes("#define HSHADER_COUNT " + vlist.size() + "\n\n");
                for (int i = 0;i < vlist.size();i++){
                    cout.writeBytes("#define HSHADER_" + namelist.get(i).toUpperCase() + " " + i +"\n");
                }
            } catch(Exception e){
                
            } finally {
                cout.close();
            }
            
			DataOutputStream javaout = new DataOutputStream(new FileOutputStream("HIShaders.java"));
			try{
				javaout.writeBytes("// automatically generated interface\n");
				javaout.writeBytes("package com.dwtech.android.houyi.shader;\n\n");
				javaout.writeBytes("public interface HIShaders {\n");
				javaout.writeBytes("\tint HSHADER_COUNT = " + vlist.size() + ";\n\n");
				for (int i = 0;i < vlist.size();i++){
					javaout.writeBytes("\tint HSHADER_" + namelist.get(i).toUpperCase() + " = " + i +";\n");	
				}
				javaout.writeBytes("}\n");
			} finally {
				javaout.close();
			}
		} catch (Exception e) {
			e.printStackTrace();
		}
	}
}
