// see http://stackoverflow.com/questions/4801655/how-to-go-to-a-specific-element-on-page
//
(function($) {
    $.fn.goTo = function() {
        $('html, body').animate({
            scrollTop: $(this).offset().top + 'px'
        }, 'fast');
        return this; // for chaining...
    }
})(jQuery);

// usage: $('#myelement').goTo();
