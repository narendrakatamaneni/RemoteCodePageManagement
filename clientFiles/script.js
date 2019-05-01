var commentsHidden= true;
var classesHidden=true;
var functionHidden=true;

function togglevisibility(classname){
	var elements= document.getElementsByClassName(classname);
	if ( classname="comments"){

		if (commentsHidden==false) commentsHidden= true;
		else commentsHidden= false;
		var i;
		for (i=0;i<elements.length;i++){
			elements[i].hidden = commentsHidden;
		}
    }

    if (classname = "class") {

        if (classesHidden == false) classesHidden = true;
        else classesHidden = false;
        var i;
        for (i = 0; i < elements.length; i++) {
            elements[i].hidden = classesHidden;
        }
    }

    if (classname = "function") {

        if (functionHidden == false) functionHidden = true;
        else functionHidden = false;
        var i;
        for (i = 0; i < elements.length; i++) {
            elements[i].hidden = functionHidden;
        }
    }
}