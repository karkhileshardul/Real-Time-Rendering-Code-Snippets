// global variables
var canvas=null;
var context=null;

function main()
{
    canvas = document.getElementById("AMC");
    if(!canvas)
        console.log("Obtaining Canvas Failed\n");
    else
        console.log("Obtaining Canvas Succeeded\n");
    
    console.log("Canvas Width: "+canvas.width+" And Canvas Height: "+canvas.height);

    context=canvas.getContext("2d");
    if(!context)
        console.log("Obtaining 2D Context Failed\n");
    else
        console.log("Obtaining 2D Context Succeeded\n");
    
    context.fillStyle="black"; // #000000
    context.fillRect(0,0,canvas.width,canvas.height);
    
    context.textAlign="center";
    context.textBaseline="middle";

    var str="Hello World !!!";
    
    context.font="48px sans-serif";
    
    context.fillStyle="white"; // #FFFFFF
    
    context.fillText(str,canvas.width/2,canvas.height/2);
    
    window.addEventListener("keydown", keyDown, false);
    window.addEventListener("click", mouseDown, false);
}

function keyDown(event)
{
    alert("A Key Is Pressed");
}

function mouseDown()
{
    alert("Mouse Is Clicked");
}
