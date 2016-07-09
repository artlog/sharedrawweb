
function getXMLHttpRequestObject(){
     var activexmodes=["Msxml2.XMLHTTP", "Microsoft.XMLHTTP"]; //activeX versions to check for in IE
     if (window.ActiveXObject){ //Test for support for ActiveXObject in IE first (as XMLHttpRequest in IE7 is broken)
      for (var i=0; i<activexmodes.length; i++){
       try{
        return new ActiveXObject(activexmodes[i]);
       }
       catch(e){
        //suppress error
       }
      }
     }
     else if (window.XMLHttpRequest) // if Mozilla, Safari etc
      return new XMLHttpRequest();
     else
      return false;
}
 
function XMLHttpRequestGetCompressedImage(){
    var mygetrequest=getXMLHttpRequestObject();
    if ( mygetrequest )
    {
	mygetrequest.onreadystatechange=function(){
	    if (mygetrequest.readyState==4){
		if (mygetrequest.status==200 || window.location.href.indexOf("http")==-1){
		    document.getElementById("result").innerHTML = "<pre>" + mygetrequest.getAllResponseHeaders() + "</pre>";
		    receiveXMLHttpRequestResponse(mygetrequest);
		}
		else{
		    alert("An error has occured making the request");
		}
	    }
	}
	//var userId = encodeURIComponent(document.getElementById("userId").value);
	//var password = encodeURIComponent(document.getElementById("password").value);
	//	mygetrequest.open("GET", "http://localhost/tests/images/defaultimg.imc", true);
	mygetrequest.open("GET", "/tests/images/defaultimg.imc", true);
	//mygetrequest.responseType="blob";
	mygetrequest.responseType="arraybuffer";
	mygetrequest.send(null);
    }
}

function receiveXMLHttpRequestResponse(myrequest)
{
    drawrequest(myrequest);
}

function XMLHttpRequestPostCompressedImage(image)
{
  var client = getXMLHttpRequestObject();
  client.open("POST", "/tests/images/newimage.imc");
  client.setRequestHeader("Content-Type", "application/octet-stream");
  client.send(image);
}
