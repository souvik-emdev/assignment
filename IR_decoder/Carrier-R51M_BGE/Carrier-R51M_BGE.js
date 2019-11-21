/*
v1.0
Encoder files for Carrier-R51M_BGE
Protocol : ELECTROLUX96_192AC
AC model : Carrier R51M/BGE
*/

var ir_out2 = [];

const tempOptions = {
    "17": 0x0,
    "172": 0xF0,
    "18": 0x10,
    "182": 0xE0,
    "19": 0x30,
    "192": 0xC0,
    "20": 0x20,
    "202": 0xD0,
    "21": 0x60,
    "212": 0x90,
    "22": 0x70,
    "222": 0x80,
    "23": 0x50,
    "232": 0xA0,
    "24": 0x40,
    "242": 0xB0,
    "25": 0xC0,
    "252": 0x30,
    "26": 0xD0,
    "262": 0x20,
    "27": 0x90,
    "262": 0x20,
    "28": 0x80,
    "282": 0x70,
    "29": 0xA0,
    "290": 0x50,
    "30": 0xB0,
    "302": 0x40
};

const fanOptions = {
    "low": 0x90,
    "low2": 0x60,
    "med": 0x50,
    "med2": 0xA0,
    "high": 0x30,
    "high2": 0xC0,
    "auto": 0xB0,
    "auto2": 0x40
};


function setSwing() {
    ir_out2[1] = 0xB2;
    ir_out2[2] = 0x4D;
    ir_out2[3] = 0x6B;
    ir_out2[4] = 0x94;
    ir_out2[5] = 0xE0;
    ir_out2[6] = 0x1F;
}

function setPowerOff() {

    ir_out2[1] = 0xB2;
    ir_out2[2] = 0x4D;
    ir_out2[3] = 0x7B;
    ir_out2[4] = 0x84;
    ir_out2[5] = 0xE0;
    ir_out2[6] = 0x1F;
}


function setMode(mode, change) {
    if (change != 4) {
        switch (mode) {

            case 1:             //cool
                ir_out2[5] &= 0xF0;
                ir_out2[6] &= 0xF0;
                ir_out2[6] |= 0x0F;
                break;
            case 2:             //auto, fan fixed
                ir_out2[3] &= 0x0F;
                ir_out2[3] |= 0x10;
                ir_out2[4] &= 0x0F;
                ir_out2[4] |= 0xE0;
                ir_out2[5] &= 0xF0;
                ir_out2[5] |= 0x08;
                ir_out2[6] &= 0xF0;
                ir_out2[6] |= 0x07;
                break;
            case 3:             //fan, temp fixed
                ir_out2[5] = 0xE4;
                ir_out2[6] = 0x1B;
                break;
            case 4:             //heat
                ir_out2[5] &= 0xF0;
                ir_out2[5] |= 0x0C;
                ir_out2[6] &= 0xF0;
                ir_out2[6] |= 0x03;
                break;
            case 5:             //dry, fan fixed
                ir_out2[3] &= 0x0F;
                ir_out2[3] |= 0x10;
                ir_out2[4] &= 0x0F;
                ir_out2[4] |= 0xE0;
                ir_out2[5] &= 0xF0;
                ir_out2[5] |= 0x04;
                ir_out2[6] &= 0xF0;
                ir_out2[6] |= 0x0B;
        }
    }
}

function fillRepeatArray() {

    for (var k = 7; k < 13; k++) {
        ir_out2[k] = ir_out2[k - 6];
    }
}

function ir_generator(irInfo) {

    if (irInfo.change == 4) {

        setSwing();
    }

    else if (irInfo.change == 1 && irInfo.power == 0) {

        setPowerOff();

    }

    else {
        ir_out2[1] = 0xB2;
        ir_out2[2] = 0x4D;
        ir_out2[3] = 0x0F;
        ir_out2[4] = 0x0;
        ir_out2[5] = 0x0;
        ir_out2[6] = 0x0F;

        //set temp
        ir_out2[5] |= tempOptions[irInfo.temp.toString()];
        ir_out2[6] |= tempOptions[irInfo.temp.toString().concat('2')];

        //set fan
        ir_out2[3] |= fanOptions[irInfo.fan.toString()];
        ir_out2[4] |= fanOptions[irInfo.fan.toString().concat('2')];

    }

    setMode(irInfo.mode, irInfo.change);

    fillRepeatArray();

    ir_out2[0] = 0x60; // Length to consider

    console.log('\nFormed Data');
    var stringer = "[";
    ir_out2.forEach(function (element) {
        stringer += ('0x' + element.toString(16) + ", ");
    });
    stringer += "]";
    console.log(stringer);
}

var xyz = {};
xyz.power = 1;
xyz.temp = 18; //range 17-30
xyz.fan = "auto"; //1, 2, 3, or auto
//xyz.hsw = 1; not available in this model
xyz.vsw = 0;
xyz.mode = 5; //Cool=1, auto=2, fan=3, heat=4, dry=5
xyz.change = 3;
ir_generator(xyz);