const net = require('net');

// Simple HTTP server responds with a simple WebSocket client test
const httpServer = net.createServer((connection) => {
    connection.on('data', () => {
        let content = `<!DOCTYPE html>
<html>
  <head>
    <meta charset="UTF-8" />
  </head>
  <body>
    <h1>WebSocket test page</h1>
    <div class = "chat">
    <textarea id = "message">Input your messages here</textarea>
    <textarea readonly id = "reply"></textarea>
    </div>
    <button onClick = "sendMessage()">Send message</button>
    <script>
    function sendMessage(){
        let mes = document.getElementById("message").value
        ws.send(mes);
    }
      let ws = new WebSocket('ws://localhost:3001');
      ws.onmessage = (event) => {
            document.getElementById("reply").value += (event.data + "\n");
      };
      ws.onopen = () => {
          ws.send("Hello");
          console.log("Open");
      }
      ws.onclose = () => console.log("Closing...");
      
      
     
      
    </script>
    
    <style>
    .chat {
    display:flex;
    flex-direction: row;
    gap: 50px;
    }
    textarea{
    height: 500px;
    width:500px;
    resize: none;
    }
    </style>
  </body>
</html>
`;
        connection.write('HTTP/1.1 200 OK\r\nContent-Length: ' + content.length + '\r\n\r\n' + content);
    });
});
httpServer.listen(3000, () => {
    console.log('HTTP server listening on port 3000');
});