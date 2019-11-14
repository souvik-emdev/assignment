var ir_out2 = [];

function setswing(change, hsw, vsw) {
    if (change == 4) {
        if (hsw) { ir_out2[2] = 0x13; ir_out2[3] = 0x16; }
        else { ir_out2[2] = 0x13; ir_out2[3] = 0x17; }
    }
    else if (change == 5) {
        if (vsw) { ir_out2[2] = 0x13; ir_out2[3] = 0x14; }
        else { ir_out2[2] = 0x13; ir_out2[3] = 0x15; }
    }
}

function setfan(fan) {
    ir_out2[3] = (ir_out2[3] & 0xF0);
    if (fan == 1) { ir_out2[3] = (ir_out2[3] | 0x00); }
    else if (fan == 2) { ir_out2[3] = ir_out2[3] | 0x02; }
    else if (fan == 3) { ir_out2[3] = ir_out2[3] | 0x04; }
    else if (fan == 4) { ir_out2[3] = ir_out2[3] | 0x05; }

}

function settemp(temp) {
    ir_out2[3] = (ir_out2[3] & 0x0F);
    var val = 0x10;
    for (var i = 16; i <= 30; i++) {
        if (i == temp) { break; }
        val += 10;
    }
    ir_out2[3] = val | ir_out2[3];
}

function setmode(mode) {
    if (mode == 1) { ir_out2[2] = 0x08; } //cool
    else if (mode == 2) { ir_out2[2] = 0x0B; settemp(17); setfan(4); } //auto, set temp & fan
    else if (mode == 3) { ir_out2[2] = 0x09; } //dry
    else if (mode == 4) { ir_out2[2] = 0x0A; settemp(18); } //fan, set temp
}


function summation() {
    var sum = 0;
    ir_out2[4] = 0;
    for (var k = 1; k < 4; k++) {
        // Serial.println(k);
        var nibble1 = (ir_out2[k] & 0x0F) & 0xFF;
        var nibble2 = ((ir_out2[k] & 0xF0) >> 4) & 0xFF;
        sum += (nibble1 + nibble2);
    }
    ir_out2[4] = ((sum & 0x0F) << 4) & 0xFF;
    //   Serial.println("");
    //   Serial.print("CheckSum:");
    //   Serial.println(checksum, HEX);  
}

function setpower(power, temp, fan, mode) {
    if (power) {
        settemp(temp);
        setfan(fan);
        if (mode == 1) { ir_out2[2] = 0x0; }
        else if (mode == 2) { ir_out2[2] = 0x03; }
        else if (mode == 3) { ir_out2[2] = 0x01; }
        else if (mode == 4) { ir_out2[2] = 0x02; }
    }
    else { ir_out2[2] = 0xC0; ir_out2[3] = 0x05; }
}



function ir_generator(irInfo) {

    var i = 0;
    var swingInfo = {};

    ir_out2[i++] = 0x1C;
    ir_out2[i++] = 0x88;

    switch (irInfo.change) {
        case 1: ;
            setpower(irInfo.power, irInfo.temp, irInfo.fan, irInfo.mode);
            break;

        case 4:
        case 5:
            swingInfo.change = irInfo.change;
            swingInfo.hsw = irInfo.hsw;
            swingInfo.vsw = irInfo.vsw;

            setswing(swingInfo);
            break;

        default: ;
            settemp(irInfo.temp);
            setfan(irInfo.fan);
            setmode(irInfo.mode);
            break;
    }

    summation();

    console.log(ir_out2);

    console.log('\nFormed Data');
    var stringer = "[";
    ir_out2.forEach(function (element) {
        stringer += ('0x' + element.toString(16) + ", ");
    });
    stringer += "]";
    console.log(stringer);

}


var xyz = {};
xyz.power = 0;
xyz.temp = 16;
xyz.fan = 4;
xyz.hsw = 0;
xyz.vsw = 0;
xyz.mode = 1;
xyz.change = 1;

ir_generator(xyz);