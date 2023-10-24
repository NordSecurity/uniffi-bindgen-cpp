{%- match config.custom_types.get(name.as_str()) %}
{%- when Some with (config) %}
{%- when None %}
typedef {{ builtin|type_name }} {{ name }};
{%- endmatch %}
