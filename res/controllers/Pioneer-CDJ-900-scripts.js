
function PioneerCDJ900() {}

PioneerCDJ900.debug = true;

PioneerCDJ900.init = function(id, debugging) {

//    for (var i = 1; i <= 65000; i++) { // Repeat the following code for the numbers 1 through 40
//                                // see https://developer.mozilla.org/en-US/docs/Web/JavaScript/Reference/Statements/for
//		print('led ' + i);
//        midi.sendShortMsg(0x90, i, 0x7f)
//        midi.sendShortMsg(0x90, i, 0x00);
//    }
}
PioneerCDJ900.shutdown = function(id) {}

/**
 * Direction (Reverse)
 */
PioneerCDJ900.reverse_play = false;

PioneerCDJ900.reverse_toggle = function(channel, control, value, status, group) {
    if (value == 0)
        return;
    if (PioneerCDJ900.reverse_play)
       PioneerCDJ900.reverse_play = false;
    else
       PioneerCDJ900.reverse_play = true;
    engine.setValue(group,'reverse',PioneerCDJ900.reverse_play);
}

/**
 * Jog
 */
PioneerCDJ900.jog_sensitivity = 2.0;
PioneerCDJ900.scratch_sensitivity = 1.0;

PioneerCDJ900.jog_wheel = function(channel, control, value, status, group) {
    value = (value-0x40) / PioneerCDJ900.jog_sensitivity;
    engine.setValue(group,'jog',value);
};

PioneerCDJ900.jog_scratch = function(channel, control, value, status, group) {
    value = (value-0x40) / PioneerCDJ900.scratch_sensitivity;
    engine.setValue(group,'jog',value);
};

PioneerCDJ900.vinyl_speed_adjust = function(channel, control, value, status, group) {
	humanArguments(arguments);
//    value = (value-0x40) / PioneerCDJ900.scratch_sensitivity;
//    engine.setValue(group,'jog',value);
};


/**
 * Hot Cues
 */
PioneerCDJ900.last_hotcue = 0;

PioneerCDJ900.hotcue_activate = function(group,hotcue,value) {
    PioneerCDJ900.last_hotcue = hotcue;
    var key = 'hotcue_' + hotcue + '_activate';
    engine.setValue(group,key,value);
}

PioneerCDJ900.hotcue_1_activate = function(channel, control, value, status, group) {
    PioneerCDJ900.hotcue_activate(group,1,value);
}

PioneerCDJ900.hotcue_2_activate = function(channel, control, value, status, group) {
    PioneerCDJ900.hotcue_activate(group,2,value);
}

PioneerCDJ900.hotcue_3_activate = function(channel, control, value, status, group) {
    PioneerCDJ900.hotcue_activate(group,3,value);
}

PioneerCDJ900.hotcue_4_activate = function(channel, control, value, status, group) {
    PioneerCDJ900.hotcue_activate(group,4,value);
}

PioneerCDJ900.hotcue_last_delete = function(channel, control, value, status, group) {
    if (PioneerCDJ900.last_hotcue < 1 || PioneerCDJ900.last_hotcue > 4)
        return;
    var key = 'hotcue_' + PioneerCDJ900.last_hotcue + '_clear';
    engine.setValue(group,key,value);
    PioneerCDJ900.last_hotcue = 0;
}

/**
 * Tempo button
 */
PioneerCDJ900.tempo_range = function(channel, control, value, status, group) {
    if (value == 0x00) {
        return;
    }

    var oldValue = engine.getValue(group, "rateRange");
    var newValue = 0.06;

    if (oldValue == 1) {
        newValue = 0.06;
    } else if (oldValue == 0.06) {
        newValue = 0.10;
    } else if (oldValue == 0.10) {
        newValue = 0.16;
    } else if (oldValue == 0.16) {
        newValue = 1.00;
    }

    engine.setValue(group, "rateRange", newValue);
};


/**
 * Playlist
 */

PioneerCDJ900.playlistMode = false;

PioneerCDJ900.select_turn = function(channel, control, value, status, group) {
    // CCW:  98 -> 127
    // CW:    1 ->  30

    if (value > 64) {
        value -= 128;
    }

    if (PioneerCDJ900.playlistMode) {
        engine.setValue("[Playlist]", "SelectTrackKnob", value);
    } else {
        if (value > 0) {
            engine.setValue("[Playlist]", "SelectNextPlaylist", 1);
        } else if (value < 0) {
            engine.setValue("[Playlist]", "SelectPrevPlaylist", 1);
        }
    }
};

PioneerCDJ900.doubleClickTimers = [];
PioneerCDJ900.isDoubleClick = function(callerId, callback) {

	if (!(callerId in this.doubleClickTimers)) {
		PioneerCDJ900.forgetDoubleClick(callerId);
		this.doubleClickTimers[callerId] = engine.beginTimer(200, function(){
			callback(false);
			delete(PioneerCDJ900.doubleClickTimers[callerId]);
		}, true);
	} else {
		callback(true);
		PioneerCDJ900.forgetDoubleClick(callerId);
	}
}

PioneerCDJ900.forgetDoubleClick = function(callerId) {
	if (!(callerId in this.doubleClickTimers))
		return;
	engine.stopTimer(this.doubleClickTimers[callerId]);
	delete(this.doubleClickTimers[callerId]);
}



PioneerCDJ900.select_push = function(channel, control, value, status, group) {
    if (value == 0x00) {
        return;
    }

	if (PioneerCDJ900.playlistMode) {
		engine.setValue(group, "LoadSelectedTrack", 1);
	} else {
		PioneerCDJ900.isDoubleClick('select_push'+channel, function(isDouble){
			if (isDouble) {
				// open the playlist
				PioneerCDJ900.playlistMode = true;
			} else {
				// (un)fold current playlist
				engine.setValue("[Playlist]", "ToggleSelectedSidebarItem", true);
			}
		});
	}
};

PioneerCDJ900.select_back = function(channel, control, value, status, group) {
    PioneerCDJ900.playlistMode = false;
};

PioneerCDJ900.toggle_playlist = function(channel, control, value, status, group) {
    if (value == 0x00) {
        return;
    }
    PioneerCDJ900.playlistMode = !PioneerCDJ900.playlistMode;
};


/**
 * Utils
 */
humanArguments = function(args) {
	var argumentsNames = [
		'channel',
		'control',
		'value',
		'status',
		'group'
	];

//	print(args.length);
	var i = 0;
	var out = [];
	for (i; i < args.length; i++) {
//		print(i);
		out.push(argumentsNames[i] + ' : ' + args[i]);
	}
	return '[' + out.join(', ') + ']';
}


