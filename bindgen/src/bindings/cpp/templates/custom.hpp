{%- match config.custom_types.get(name.as_str()) %}
{%- when Some with (type_config) %}
{% match type_config.imports %}
{% when Some with (includes) %}
{% for include in includes %}
{{ self.add_include(include) }}
{% endfor %}
{% else %}
{% endmatch %}
{%- when None %}
typedef {{ builtin|type_name }} {{ name }};
{%- endmatch %}
