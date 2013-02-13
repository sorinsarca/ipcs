(function($){
    $(document).ready(function(){
        var last_load = Drupal.settings.ipcs_test.count;
        $('#sidebar_nodes_load').click(function(e){
           $.ajax({
            data: {
                action: 'get_nodes',
                start: last_load,
                count: Drupal.settings.ipcs_test.count
            },
            dataType: 'json',
            url: Drupal.settings.ipcs_test.ajax_path,
            success: function(data) {
                var container = $('#sidebar_nodes');
                while (data.length) {
                    container.append(data.shift());
                }
            }
           });
           last_load += Drupal.settings.ipcs_test.count;
        });
    });
})(jQuery);