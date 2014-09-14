// This function displays a popup message
function displayMessage(elem, msg, style) {
    $(elem).html(
        '<div class="alert fade in ' + style + '" role="alert">' +
        '  <button type="button" class="close" data-dismiss="alert">' +
        '    <span aria-hidden="true">&times;</span>' +
        '    <span class="sr-only">Close</span>' +
        '  </button>' +
           msg +
        '</div>');
}
