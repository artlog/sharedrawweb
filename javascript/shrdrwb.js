// ported from java code https://github.com/artlog/sharedrawweb

function record(thisline) {
    var b_context = canvas2.getContext("2d");
    if ( thisline.length > 0 )
    {
	if ( debugcontent) { alert(output_line(thisline)); };
	var compressed=do_compress(thisline);
	if ( debugcontent) { alert(output_chunk(compressed)); };
	var intbuffer=tobytestream(compressed);
	if ( debugcontent) {alert(output_int(intbuffer));};
	var fieldreader=null;
	if ( debugcontent)
	{
	    fieldreader=frombytestream(intbuffer,compressed);
	}
	else
	{
	    fieldreader=frombytestream(intbuffer);
	}
	thisline=[];
	thisline=do_expand(fieldreader);
	if ( debugcontent) { alert(output_line(thisline)); };
	if (thisline.length > 0)
	{
	    var o=thisline[0];
	    b_context.beginPath();
	    b_context.moveTo(o.x, o.y);
	    for (var i=1; i<thisline.length; i++)
	    {
		var o=thisline[i];
		b_context.lineTo(o.x, o.y);
	    }
	    b_context.stroke();
	}
    }
}

function draw_b(event) {
    var x = event.pageX - canvas.offsetLeft;
    var y = event.pageY - canvas.offsetTop;
    if ( ! drawit )
    {
	lastx=x;
	lasty=y;
	record(myline);
	myline=[];
	return;
    }
    var b_context = canvas.getContext("2d");
    b_context.beginPath();
    b_context.moveTo(lastx, lasty);
    b_context.lineTo(x, y);
    var o = {x:x,y:y};
    myline.push(o);
    b_context.stroke();
    lastx=x;
    lasty=y;
    canvas.render();
}

function output_line( fromLines )
{
    var output_text="";
    if (fromLines.length > 0)
    {
	for (var i=0; i < fromLines.length; i++) {
	    var frompoint = fromLines[i];
	    output_text += "(x=" + frompoint.x + ",y=" + frompoint.y + ")";
	}
    }
    return output_text;

}

function output_int( intArray )
{
    var output_text="";
    if (intArray.length > 0)
    {
	for (var i=0; i < intArray.length; i++) {
	    var frompoint = intArray[i];
	    output_text += " " + frompoint + ",";
	}
    }
    return output_text;
}

function output_chunk( fromChunks )
{
    var output_text="";
    if (fromChunks.length > 0)
    {
	for (var i=0; i < fromChunks.length; i++) {
	    var frompoint = fromChunks[i];
	    output_text += "(size=" + frompoint.size + ",code=" + frompoint.code + ")";
	}
    }
    return output_text;
}


function do_compress( fromLines )
{
    var skipsame=0;
    var frompoint=null;
    var topoint=null;
    var thispoint=null;
    var x=0, y=0, center=0, thissize=0;

    fieldWriter=[];
    
    if (fromLines.length > 1)
    {
	for (var i=1; i < fromLines.length; i++) {
	    frompoint = fromLines[i];
	    if ( ( topoint != null )  && ( topoint.x == frompoint.x ) && ( topoint.y == frompoint.y ) )
		{
		    skipsame ++;
		}
	    topoint=frompoint;
	}
    }

    if ( fromLines.length > 0)
    {
	frompoint = fromLines[0];
	topoint={x:0,y:0};
	var newlength=(fromLines.length - skipsame);
	writeAbs( frompoint, 64);
	for ( var i=1; i < fromLines.length; i++) {
            thispoint = fromLines[i];
            topoint.x = thispoint.x - frompoint.x;
            topoint.y = thispoint.y - frompoint.y;
	    if (( topoint.x == 0 ) && ( topoint.y == 0 ))
	    {
		continue;
	    }
            if ( ( topoint.x < 2) && (topoint.y < 2) && (topoint.x > -2 ) && (topoint.y > -2) )
	    {
		writeRel( topoint, 3);
            }
            else {
		thissize = 64;
		for ( var index=1; index < ( SCODE_MAX - 1 ); index ++) {
		    center = 1 << ( (  scode[index]  / 2 )  - 1 );
		    if ( ( topoint.x <= center  ) && ( topoint.x > -center )
			 && ( topoint.y <= center  ) && ( topoint.y > -center ) )
		    {
			thissize = scode[index];
			break; 
		    }
		}
		if ( thissize < 32 ) {
		    writeRel( topoint, thissize);
		}
		else {
		    // warning absolute
		    writeRel( thispoint, 64);
		}
            }
            // new frompoint is previous point
            frompoint = thispoint;
	}
    }

    return fieldWriter;
}

function writeAbs( thispoint, thissize) {
    var codeval=0;
    var center=0, max=0;

    previousSize = thissize;

    if ( thissize > 32 ) {
	fieldWriter.push( {code:thispoint.x,size:32});
	fieldWriter.push( {code:thispoint.y,size:32});
    }
    else {
	var nocenter = false;
	if ( thissize == 3 ) {
            max = 3;
	    center = 1;
	    nocenter = true;
	}
	else {
            max = ( 1 << (  thissize  / 2 ) );
	    center = (max / 2) - 1;
	}
	codeval = thispoint.x + center +  (( thispoint.y + center ) * max );
	if (nocenter) {
	    if ( codeval > (center * max)  ){
		codeval --;
	    }
	}
	fieldWriter.push( {size:thissize, code:codeval});
    }
}

function writeRel( thispoint, thissize) {

    // bypass length encoding, yet stored
    if ( encodeLength )
    {
	if ( thissize != previousSize) {
	    var cindex=0, pindex=0;
	    for ( cindex = 0; cindex < SCODE_MAX; cindex ++) {
		if ( scode[cindex] == thissize ) break;
	    }
	    for ( pindex = 0; pindex < SCODE_MAX; pindex ++) {
		if ( scode[pindex] == previousSize ) break;
	    }
	    if ( pindex < cindex ) {
		if ( ((SCODE_MAX - cindex) + pindex ) > ( pindex - cindex ) ) {
		    for ( ; pindex < cindex; pindex ++) {
			fieldWriter.push( {code:3,size:2});
		    }
		}
		else {
		    for ( pindex = SCODE_MAX - cindex + pindex; pindex > 0; pindex --) {
			fieldWriter.push( {code:2,size:2});
		    }
		}
	    }
	    else {
		if ( ((SCODE_MAX - pindex) + cindex ) > cindex - pindex ) {
		    for ( ; cindex < pindex; pindex --) {
			fieldWriter.push( {code:2,size:2});
		    }
		}
		else {
		    for ( pindex = SCODE_MAX - pindex + cindex; pindex > 0; pindex --)
		    {
			fieldWriter.push( {code:3,size:2});
		    }
		}
	    }
	}  
	fieldWriter.push( {code:0,size:1});
	previousSize=thissize;
    }
    writeAbs( thispoint, thissize);
}

function tobytestream(fieldReader)
{
    var bstream=newbstream();
    writestream(bstream,fieldReader.length-1,32);
    previousSize=64;
    for (var i=0; (i < 2) && (i < fieldReader.length); i++)
    {
	var chunk=fieldReader[i];
	writestream(bstream,chunk.code,32);
    }
    for (var i=2; i < fieldReader.length; i++)
    {
	var chunk=fieldReader[i];
	var thissize=chunk.size;

	if ( thissize != previousSize) {
	    var cindex=0, pindex=0;
	    for ( cindex = 0; cindex < SCODE_MAX; cindex ++) {
		if ( scode[cindex] == thissize ) break;
	    }
	    for ( pindex = 0; pindex < SCODE_MAX; pindex ++) {
		if ( scode[pindex] == previousSize ) break;
	    }
	    if ( pindex < cindex ) {
		if ( ((SCODE_MAX - cindex) + pindex ) > ( pindex - cindex ) ) {
		    for ( ; pindex < cindex; pindex ++) {
			writestream(bstream,3,2);
		    }
		}
		else {
		    for ( pindex = SCODE_MAX - cindex + pindex; pindex > 0; pindex --) {
			writestream(bstream,2,2);
		    }
		}
	    }
	    else {
		if ( ((SCODE_MAX - pindex) + cindex ) > cindex - pindex ) {
		    for ( ; cindex < pindex; pindex --)
		    {
			writestream(bstream,2,2);
		    }
		}
		else {
		    for ( pindex = SCODE_MAX - pindex + cindex; pindex > 0; pindex --)
		    {
			writestream(bstream,3,2);
		    }
		}
	    }
	}
	writestream(bstream,0,1);
	writestream(bstream,chunk.code,thissize);
	previousSize=thissize;
    }
    padToWord(bstream);
    return bstream.buffer;
}

function newbstream()
{
    return {bitOffset:0,nextWord:0,buffer:[]};
}

function newbstreamread()
{
    return {bitOffset:0,currentWord:0};
}

function writestream( bstream, field, bits)
{
    // if more bits needed that word currently used.
    if ( bits  + bstream.bitOffset > DATAWORDSIZE) {
	// warning recursive call ( not terminal )
	var bitsize = DATAWORDSIZE - bstream.bitOffset;
	var head = field >> (bits - bitsize);
	writestream( bstream, head, DATAWORDSIZE - bstream.bitOffset);
	field = field & ( UNSIGNEDWORDMAX >> ( bitsize - 1));
	writestream( bstream, field, bits - bitsize);
    }
    else {         
	if ( bits == DATAWORDSIZE ) {
            // special case to keep sign
            bstream.nextWord = field; 
            bstream.bitOffset = 0;
	}
	else {
            // keep only meaning part of field
            field = field & ( UNSIGNEDWORDMAX >> ( DATAWORDSIZE - 1 - bits));
            bstream.nextWord = bstream.nextWord | ( field << ( DATAWORDSIZE - bits - bstream.bitOffset));
            bstream.bitOffset = ( bstream.bitOffset + bits ) % DATAWORDSIZE; 
	}       
	// a new word is needed
	if ( bstream.bitOffset == 0 ) {
            newWord(bstream);
	}
    }
}

function newWord(bstream)
{
    bstream.buffer.push(bstream.nextWord);
    bstream.bitOffset = 0;
    bstream.nextWord = 0;
}

/*
  write all current datas if any
  to have a clean word ready.
*/
function padToWord(bstream)
{
    if ( bstream.bitOffset != 0) {
	newWord(bstream);
    }
}

function frombytestream(intArray,checkArray)
{
    var fieldWriter = [];
    var bstream=newbstreamread()
    var npts = readstreambits(bstream,32,intArray);
    // read first point
    fieldWriter.push({code:readstreambits(bstream,32,intArray),size:32});
    fieldWriter.push({code:readstreambits(bstream,32,intArray),size:32});
    var currentsize=64;
    for (var point=0; point < (npts-1); point ++)
    {
	var index;
	var codeval=0;
	for ( index = 0; index < SCODE_MAX; index ++) {
	    if ( scode[index] == currentsize ) break;
	}
	while ( readstreambits( bstream, 1, intArray) != 0 ) {
	    if (  readstreambits( bstream, 1, intArray) == 0 )
	    {
		index --;
	    }
	    else
	    {
		index++;
	    }
	    if ( index > SCODE_MAX ) {
		index = 0;
	    }
	    if ( index < 0 ) {
		index = SCODE_MAX;
	    }
	}
	currentsize=scode[index];
	if ( checkArray )
	{
	    var chunk=checkArray[2+point];
	    if ( chunk.size != currentsize )
	    {
		alert("[" + point + "] (size=" + currentsize + ",code=#undef != " + "(size=" + chunk.size + ",code=" + chunk.code + "),(offset=" + bstream.bitOffset + ",word=" + bstream.currentWord + ")");
		return fieldWriter;
	    }
	    codeval=readstreambits(bstream, currentsize, intArray);
	    if ( ( chunk.code != codeval ) )
	    {
		alert("[" + point + "] (size=" + currentsize + ",code=" + codeval + ") != " + "(size=" + chunk.size + ",code=" + chunk.code + ")(offset="  + bstream.bitOffset + ",word=" + bstream.currentWord + ")");
		return fieldWriter;
	    }
	}
	else
	{
	    codeval=readstreambits(bstream, currentsize, intArray);
	}
	fieldWriter.push({code:codeval,size:currentsize});

    }
    readPadToWord(bstream,intArray);
    return fieldWriter;
}


function readstreambits(bstream, bits, intArray) {
    var field = 0;
    var head = 0;
    if ( bstream.bitOffset == 0 ) {
	nextWord(bstream, intArray);
    }
    if ( bits  + bstream.bitOffset > DATAWORDSIZE) {
	var bitsize = DATAWORDSIZE - bstream.bitOffset;
	head = readstreambits(bstream, DATAWORDSIZE - bstream.bitOffset, intArray);
	field = readstreambits(bstream, bits - bitsize, intArray);
	field = field | ( head << (bits - bitsize));
    }
    else {
	if ( bits == DATAWORDSIZE) {
            field = bstream.currentWord;
            bstream.currentWord = 0;
            bstream.bitOffset = 0;
	}
	else {       
            field = ( bstream.currentWord >> ( DATAWORDSIZE - bits) )
		& ( UNSIGNEDWORDMAX >> ( DATAWORDSIZE - 1 - bits) );
            bstream.currentWord <<= bits;
            bstream.bitOffset = ( bstream.bitOffset + bits ) % DATAWORDSIZE;
	}
    }
    return field;  
} 

function nextWord(bstream, intArray) {
    bstream.currentWord=intArray.shift();
    bstream.bitOffset=0;
}

function readPadToWord(bstream, intArray) {
    if ( bstream.bitOffset != 0) {
	nextWord(bstream, intArray);
    }
}

function do_expand( fieldReader )
{
    var result=[];
    for (var i=0; i < fieldReader.length; i++)
    {
	var thispoint={x:0,y:0};
	var chunk=fieldReader[i];
	if ( ( chunk.size > 32 ) || ( ( ! encodeLength ) && ( chunk.size == 32 ))) {
	    thispoint.x = chunk.code;
	    i=i+1;
	    chunk=fieldReader[i];
	    thispoint.y = chunk.code;
	}
	else
	{
	    var relmov=readAbs(chunk);
	    if ( i > 0 )
	    {
		thispoint.x = relmov.x + result[result.length-1].x;
		thispoint.y = relmov.y + result[result.length-1].y;
	    }
	}
	result.push(thispoint);
    }
    return result;
}

function readAbs( chunk) {
    var codeval=0;
    var center=0, max=0;

    currentSize = chunk.size;

    var thispoint = {x:0,y:0};

    var nocenter=false;
    codeval = chunk.code;

    if ( currentSize == 3 ) {
        max = 3;
	center = 1;
	nocenter=true;
    }
    else {
        max = ( 1 << (  currentSize  / 2  ) );
	center = (max / 2) - 1;
    }
    if ( nocenter )
    {
	if ( codeval > ( center * max  ) ) {
	    codeval ++;
	}
    }
    thispoint.x = ( codeval  %  max) - center;
    thispoint.y = ( Math.floor(codeval   / max)) - center;
    
    return thispoint;
}

var myline=[];
var debugcontent=false;
var drawit=false;
var lastx=0;
var lasty=0;

// init_compressor start
var fieldWriter=[];
var previousSize=0;
var scode=[3,6,14,30,64];
var SCODE_MAX=4;
var currentSize=0;
var encodeLength=false;
var DATAWORDSIZE=32;
var UNSIGNEDWORDMAX=0x7FFFFFFF;
// init compressor end

document.write("ShareDrawWeb Canvas Test<br>");
var canvas = document.createElement('canvas');
canvas.id     = "CursorLayer";
canvas.width  = 400;
canvas.height = 300;
//canvas.style.zIndex   = 8;
//canvas.style.position = "absolute";
canvas.style.border   = "1px solid";
document.body.appendChild(canvas);
// Add event listener for `click` events.
canvas.addEventListener('click', function(event) { drawit = ! drawit }, false);
canvas.addEventListener('mousemove', function(event) {
    draw_b(event);
}, false);
document.write("<br>OUTPUT<br>");
var canvas2= document.createElement('canvas');
canvas2.id     = "CompressCheck";
canvas2.width  = 400;
canvas2.height = 300;
//canvas.style.zIndex   = 8;
//canvas.style.position = "absolute";
canvas2.style.border   = "1px solid";
document.body.appendChild(canvas2);

