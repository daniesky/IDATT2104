package ntnu.idatt2105.compilerspring.controllers;

import ntnu.idatt2105.compilerspring.models.Code;
import org.springframework.web.bind.annotation.CrossOrigin;
import org.springframework.web.bind.annotation.PostMapping;
import org.springframework.web.bind.annotation.RequestBody;
import org.springframework.web.bind.annotation.RestController;

import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.nio.charset.StandardCharsets;

@RestController
@CrossOrigin(origins = "http://localhost:8080")
public class CompileController {

    @PostMapping("/compile")
    public String receiveAndComp(@RequestBody Code code){
        return compile(code);
    }


    public String compile(Code code){
        String output = "";
        String[] command = {
                "docker", "run", "--rm", "python:latest", "python", "-c", code.getCode()
        };

        try{
            ProcessBuilder pb = new ProcessBuilder(command);
            pb.redirectErrorStream(true);
            Process p = pb.start();

            p.getOutputStream().write(code.getCode().getBytes());
            p.getOutputStream().close();

            BufferedReader br = new BufferedReader(new InputStreamReader(p.getInputStream()));
            String line;
            while((line = br.readLine()) != null){
                output += line + "\n";
            }

            int exit = p.waitFor();
            if(exit != 0){
                output += "Compilation failed with exit code " + exit;
            }

        }
        catch(Exception e){
            output += e;
        }

        return output;
    }
}
