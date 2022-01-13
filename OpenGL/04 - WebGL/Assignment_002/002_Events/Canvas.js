function main()
{
    //1. Get Canvas from DOM - Document Object Model; here HTML
    //type-inference : automatically guess the type of right hand side and use 'var' on left hand side
    var canvas = document.getElementById("KAB"); 
    if(!canvas)
    {
        console.log("Obtaining Canvas Failed! \n");
    }
    else
    {
        console.log("Obtaining Canvas Succeeded \n");
    }

    //2. Get Canvas width and height just for the sake of information
    console.log("Canvas width = "+canvas.width+" Canvas height = "+canvas.height+ "\n");

    //3. Get Drawing Context from the Canvas
    var context = canvas.getContext("2d");
    if(!context)
    {
        console.log("Obtaining Context Failed! \n");
    }
    else
    {
        console.log("Obtaining Context Succeeded \n");
    }
    
    //4. Paint Background by black colour
    context.fillStyle = "black"; //fillStyle is setter function - fillStyle is a property
    context.fillRect(0, 0, canvas.width, canvas.height); //x, y, width, height

    //5. Center the Text coming in the future
    context.textAlign = "center"; //horizontal center
    context.textBaseline = "middle"; //vertical center

    //6. Set the font size and style
    context.font = "48px sans-serif"; //UNIX convention - from Netscape NAvigator
    
    //7. Declare the String to be displayed
    var str = "Hello World !!!";

    //8. Colour the text of Foreground colour
    context.fillStyle = "white"; 

    //9. Display the text
    context.fillText(str, canvas.width / 2, canvas.height / 2);

    //Add Event Listener
    //'window' is not declared beacuse it is an in-built variable
    //'window' is inherited from 'document' --> DOM
    // "keydown" , "click" -- in build registered event of JavaScript
    // 'false'  - follow bubble propagation instead of capture
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
}


function keyDown(event)
{
    //code
    alert("Key Is Pressed");
}

function mouseDown()
{
    //code
    alert("Mouse Button Is Clicked");
}
