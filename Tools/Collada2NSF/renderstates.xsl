<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="2.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:col="http://www.collada.org/2005/11/COLLADASchema">


  <!-- Render States -->
 
  <!-- alpha_func -->
  <xsl:template match="col:alpha_func">
    <xsl:value-of select="concat($Tabs4, 'AlphaFunc = ')"/>
    <xsl:choose>
      <xsl:when test="col:func/@value!=''">
        <xsl:call-template name="FormatCompareFunc">
          <xsl:with-param name="value">
            <xsl:value-of select="col:func/@value"/>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="col:value/@value"/>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>

  <!-- blend_func -->
  <xsl:template match="col:blend_func">
    <xsl:value-of select="concat($Tabs4, 'SrcBlend = ')"/>
    <xsl:choose>
      <xsl:when test="col:src/@value!=''">
        <xsl:call-template name="FormatBlendValue">
          <xsl:with-param name="value">
            <xsl:value-of select="col:src/@value"/>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>SrcAlpha</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>&#xA;</xsl:text>
    <xsl:value-of select="concat($Tabs4, 'DestBlend = ')"/>
    <xsl:choose>
      <xsl:when test="col:dest/@value!=''">
        <xsl:call-template name="FormatBlendValue">
          <xsl:with-param name="value">
            <xsl:value-of select="col:dest/@value"/>
          </xsl:with-param>
        </xsl:call-template>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>One</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>

  <!-- blend_func_separate -->
  <xsl:template match="col:blend_func_separate">
    <xsl:if test="col:src_rgb/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'SrcBlendSeparateRGB = ')"/>
      <xsl:call-template name="FormatBlendValue">
        <xsl:with-param name="value">
          <xsl:value-of select="col:src_rgb/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
    <xsl:if test="col:dest_rgb/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'DestBlendSeparateRGB = ')"/>
      <xsl:call-template name="FormatBlendValue">
        <xsl:with-param name="value">
          <xsl:value-of select="col:dest_rgb/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
    <xsl:if test="col:src_alpha/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'SrcBlendAlpha = ')"/>
      <xsl:call-template name="FormatBlendValue">
        <xsl:with-param name="value">
          <xsl:value-of select="col:src_alpha/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
    <xsl:if test="col:dest_alpha/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'DestBlendAlpha = ')"/>
      <xsl:text> = </xsl:text>
      <xsl:call-template name="FormatBlendValue">
        <xsl:with-param name="value">
          <xsl:value-of select="col:dest_alpha/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
  </xsl:template>

  <!-- blend_equation -->
  <xsl:template match="col:blend_equation ">
    <xsl:value-of select="concat($Tabs4, 'BlendOp = ')"/>
    <xsl:call-template name="FormatBlendOp">
      <xsl:with-param name="value">
        <xsl:value-of select="@value"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>

  <!-- blend_equation_separate -->
  <xsl:template match="col:blend_equation_separate">
    <xsl:if test="col:rgb/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'BlendEquationSeparateRGB = ')"/>
      <xsl:call-template name="FormatBlendOp">
        <xsl:with-param name="value">
          <xsl:value-of select="col:rgb/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
    <xsl:if test="col:alpha/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'BlendOpAlpha = ')"/>
      <xsl:call-template name="FormatBlendOp">
        <xsl:with-param name="value">
          <xsl:value-of select="col:rgb/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
  </xsl:template>

  <!-- cull_face -->
  <xsl:template match="col:cull_face">
    <xsl:value-of select="concat($Tabs4, 'DestBlendSeparateRGB = ')"/>
    <xsl:call-template name="FormatFace">
      <xsl:with-param name="value">
        <xsl:value-of select="@value"/>
      </xsl:with-param>
    </xsl:call-template>
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>

  <!-- depth_func -->
  <xsl:template match="col:depth_func ">
    <xsl:value-of select="concat($Tabs4, 'ZFunc = ')"/>
    <xsl:call-template name="FormatCompareFunc">
      <xsl:with-param name="value">
        <xsl:value-of select="@value"/>
      </xsl:with-param>
    </xsl:call-template>
  <xsl:text>&#xA;</xsl:text>
  </xsl:template>

  <!-- front_face -->
  <xsl:template match="col:font_face">
    <xsl:value-of select="concat($Tabs4, 'FrontFace = ', @value, '&#xA;')"/>
  </xsl:template>

  <xsl:template match="col:logic_op">
    <xsl:value-of select="concat($Tabs4, 'LogicOp = ', @value, '&#xA;')"/>
  </xsl:template>
  
  <!-- polygon_mode -->
  <xsl:template match="col:polygon_mode">
    <xsl:if test="col:face/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'PolygonModeFace = ')"/>
      <xsl:call-template name="FormatFace">
        <xsl:with-param name="value">
          <xsl:value-of select="col:face/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
    <xsl:if test="col:mode/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'FillMode = ')"/>
      <xsl:call-template name="FormatPolygonMode">
        <xsl:with-param name="value">
          <xsl:value-of select="col:mode/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
  </xsl:template>

  <!-- stencil_func -->
  <xsl:template match="col:stencil_func">
    <xsl:if test="col:func/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'StencilFunc = ')"/>
      <xsl:call-template name="FormatCompareFunc">
        <xsl:with-param name="value">
          <xsl:value-of select="col:func/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
    <xsl:if test="col:ref/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'StencilRef = ', col:ref/@value, '&#xA;')"/>
    </xsl:if>
    <xsl:if test="col:mask/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'StencilMask = ', col:mask/@value, '&#xA;')"/>
    </xsl:if>
  </xsl:template>

  <!-- stencil_op -->
  <xsl:template match="col:stencil_op">
    <xsl:if test="col:fail/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'StencilFail = ')"/>
      <xsl:call-template name="FormatStencilOp">
        <xsl:with-param name="value">
          <xsl:value-of select="col:fail/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
    <xsl:if test="col:zfail/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'StencilZFail = ')"/>
      <xsl:call-template name="FormatStencilOp">
        <xsl:with-param name="value">
          <xsl:value-of select="col:zfail/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
    <xsl:if test="col:zpass/@value!=''">
      <xsl:value-of select="concat($Tabs4, 'StencilPass = ')"/>
      <xsl:call-template name="FormatStencilOp">
        <xsl:with-param name="value">
          <xsl:value-of select="col:zpass/@value"/>
        </xsl:with-param>
      </xsl:call-template>
      <xsl:text>&#xA;</xsl:text>
    </xsl:if>
  </xsl:template>

  <!-- blend_color -->
  <xsl:template match="col:blend_color">
    <xsl:call-template name="FormatColorToComponents">
      <xsl:with-param name="value">
        <xsl:value-of select="@value"/>
      </xsl:with-param>
      <xsl:with-param name="label">BlendColor</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- clear_color -->
  <xsl:template match="col:clear_color">
    <xsl:call-template name="FormatColorToComponents">
      <xsl:with-param name="value">
        <xsl:value-of select="@value"/>
      </xsl:with-param>
      <xsl:with-param name="label">ClearColor</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- clear_stencil -->
  <xsl:template match="col:clear_stencil">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'ClearStencil = ', $Value, '&#xA;')"/>
  </xsl:template>

  <!-- clear_depth -->
  <xsl:template match="col:clear_depth">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'ClearDepth = ', $Value, '&#xA;')"/>
  </xsl:template>

  <!-- color_mask -->
  <xsl:template match="col:color_mask">
    <xsl:call-template name="FormatColorToComponents">
      <xsl:with-param name="value">
        <xsl:value-of select="@value"/>
      </xsl:with-param>
      <xsl:with-param name="label">ColorMask</xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- depth_bounds -->
  <xsl:template match="col:depth_bounds">
    <xsl:call-template name="Format2">
      <xsl:with-param name="label1">DepthBoundsNear</xsl:with-param>
      <xsl:with-param name="label2">DepthBoundsFar</xsl:with-param>
      <xsl:with-param name="value">
        <xsl:value-of select="@value"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- depth_mask -->
  <xsl:template match="col:depth_mask">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'ZWriteEnable = ', $Value, '&#xA;')"/>
  </xsl:template>

  <!-- depth_range -->
  <xsl:template match="col:depth_range">
    <xsl:call-template name="Format2">
      <xsl:with-param name="label1">DepthRangeNear</xsl:with-param>
      <xsl:with-param name="label2">DepthRangeFar</xsl:with-param>
      <xsl:with-param name="value">
        <xsl:value-of select="@value"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- lighting_enable -->
  <xsl:template match="col:lighting_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'Lighting = ', $Value, '&#xA;')"/>
  </xsl:template>

  <!-- line_width -->
  <xsl:template match="col:line_width">
     <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">1</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'LineWidth = ', $Value, '&#xA;')"/>
  </xsl:template>

  <!-- point_size -->
  <xsl:template match="col:point_size">
     <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">1</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'PointSize = ', $Value, '&#xA;')"/>
  </xsl:template>
  
  <!-- point_size_min -->
  <xsl:template match="col:point_size_min">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">1</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'PointSizeMin = ', $Value, '&#xA;')"/>
  </xsl:template>
  
  <!-- point_size_max -->
  <xsl:template match="col:point_size_max">
     <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">1</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'PointSizeMax = ', $Value, '&#xA;')"/>
  </xsl:template>

  <!-- polygon_offset -->
  <xsl:template match="col:polygon_offset">
    <xsl:call-template name="Format2">
      <xsl:with-param name="label1">PolygonOffsetFactor</xsl:with-param>
      <xsl:with-param name="label2">PolygonOffsetUnits</xsl:with-param>
      <xsl:with-param name="value">
        <xsl:value-of select="@value"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- scissor -->
  <xsl:template match="col:scissor">
    <xsl:call-template name="Format4">
      <xsl:with-param name="label1">ScissorX</xsl:with-param>
      <xsl:with-param name="label2">ScissorY</xsl:with-param>
      <xsl:with-param name="label3">ScissorWidth</xsl:with-param>
      <xsl:with-param name="label4">ScissorHeight</xsl:with-param>
      <xsl:with-param name="value">
        <xsl:value-of select="@value"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>

  <!-- stencil_mask -->
  <xsl:template match="col:stencil_mask">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'StencilMask = ', $Value, '&#xA;')"/>
  </xsl:template>

  <!-- alpha_test_enable -->
  <xsl:template match="col:alpha_test_enable">
     <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'AlphaTestEnable = ', $Value, '&#xA;')"/>
  </xsl:template>

  <!-- blend_enable -->
  <xsl:template match="col:blend_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'AlphaBlendEnable = ', $Value, '&#xA;')"/>
  </xsl:template>

  <!-- logic_op_enable -->
  <xsl:template match="col:logic_op_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'ColorLogicOpEnable = ', $Value, '&#xA;')"/>
  </xsl:template>

  <!-- cull_face_enable -->
  <xsl:template match="col:cull_face_enable">
     <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'CullFaceEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>

  <!-- depth_bounds_enable -->
  <xsl:template match="col:depth_bounds_enable">
     <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'DepthBoundEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>

  <!-- depth_test_enable -->
  <xsl:template match="col:depth_test_enable">
    <xsl:value-of select="concat($Tabs4, 'ZEnable = ')"/>
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:choose>
      <xsl:when test="$Value='true'">
        <xsl:text>ZB_True</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:text>ZB_False</xsl:text>
      </xsl:otherwise>
    </xsl:choose>
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>

  <!-- dither_enable -->
  <xsl:template match="col:dither_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'DitherEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>

  <!-- line_smooth_enable -->
  <xsl:template match="col:line_smooth_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'LineSmoothEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>

  <!-- multisample_enable -->
  <xsl:template match="col:multisample_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'MultisampleEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>
  
  <!-- point_smooth_enable -->
  <xsl:template match="col:point_smooth_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'PointSmoothEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>

  <!-- polygon_offset_fill_enable -->
  <xsl:template match="col:polygon_offset_fill_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'PolygonOffsetFillEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>
  
  <!-- sample_alpha_to_coverage_enable -->
  <xsl:template match="col:sample_alpha_to_coverage_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'SampleAlphaToCoverageEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>

  <!-- sample_alpha_to_one_enable -->
  <xsl:template match="col:sample_alpha_to_one_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'SampleAlphaToOneEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>

  <!-- sample_coverage_enable -->
  <xsl:template match="col:sample_coverage_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'SampleCoverageEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>

  <!-- scissor_test_enable -->
  <xsl:template match="col:scissor_test_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'ScissorTestEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>

  <!-- stencil_test_enable -->
  <xsl:template match="col:stencil_test_enable">
    <xsl:variable name="Value">
      <xsl:call-template name="GetValue">
        <xsl:with-param name="Value" select="@value"/>
        <xsl:with-param name="Default">false</xsl:with-param>
      </xsl:call-template>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, 'StencilEnable = ', $Value,  '&#xA;')"/>
  </xsl:template>

<!-- Unsupported Render States -->
  <!-- color_material -->
  <xsl:template match="
    col:color_material|
    col:fog_mode|
    col:fog_coord_src|
    col:light_model_color_control|
    col:shade_model|
    col:stencil_func_separate|
    col:stencil_mask_separate|
    col:stencil_op_separate|
    col:light_enable| 
    col:light_ambient|
    col:light_diffuse|
    col:light_specular|
    col:light_position|
    col:light_constant|
    col:light_linear_attenuation|
    col:light_quadratic_attenuation|
    col:light_spot_cutoff|
    col:light_spot_direction|
    col:light_spot_exponent|
    col:texture1D|
    col:texture2D|
    col:texture3D|
    col:textureCUBE|
    col:textureRECT|
    col:textureDEPTH|
    col:texture1D_enable|
    col:texture2D_enable|
    col:texture3D_enable|
    col:textureCUBE_enable|
    col:textureRECT_enable|
    col:textureDEPTH_enable|
    col:texture_env_color|
    col:texture_env_mode|
    col:clip_plane|
    col:clip_plane_enable|
    col:fog_density|
    col:fog_start|
    col:fog_end|
    col:fog_color|
    col:light_model_ambient|
    col:line_stipple|
    col:material_ambient|
    col:material_diffuse|
    col:material_emission|
    col:material_shininess|
    col:material_specular|
    col:model_view_matrix|
    col:point_distance_attenuation|
    col:point_fade_threshold_size|
    col:projection_matrix|
    col:auto_normal_enable|
    col:rescale_normal_enable|
    col:color_logic_op_enable|
    col:normalize_enable|
    col:rescale_normal_enable|
    col:gl_hook_abstract|
    col:texture_pipeline|
    col:texture_pipeline_enable|
    col:depth_clamp_enable|
    col:fog_enable|
    col:light_model_local_viewer_enable|
    col:light_model_two_side_enable|
    col:line_stipple_enable|
    col:polygon_smooth_enable|
    col:polygon_offset_line_enable|
    col:polygon_offset_point_enable|
    col:polygon_stipple_enable">
    <xsl:value-of select="concat($Tabs4, '//?RenderState?:', name(), '&#xA;')"/>
  </xsl:template>

  <!-- Render State Formatters -->

  <!-- Format2 -->
  <xsl:template name="Format2">
    <xsl:param name="label1"/>
    <xsl:param name="label2"/>
    <xsl:param name="value"/>
    <xsl:variable name="first">
      <xsl:value-of select="normalize-space(substring-before($value, ' '))"/>
    </xsl:variable>
    <xsl:variable name="second">
      <xsl:value-of select="substring($value, string-length($first)+2)"/>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, $label1, ' = ', $first, '&#xA;')"/>
    <xsl:value-of select="concat($Tabs4, $label2, ' = ', $second, '&#xA;')"/>
  </xsl:template>

  <!-- FormatColorToComponents -->
  <xsl:template name="FormatColorToComponents">
    <xsl:param name="label"/>
    <xsl:param name="value"/>
    <xsl:call-template name="Format4">
      <xsl:with-param name="label1">
        <xsl:value-of select="concat($label, 'R')"/>
      </xsl:with-param>
      <xsl:with-param name="label2">
        <xsl:value-of select="concat($label, 'G')"/>
      </xsl:with-param>
      <xsl:with-param name="label3">
        <xsl:value-of select="concat($label, 'B')"/>
      </xsl:with-param>
      <xsl:with-param name="label4">
        <xsl:value-of select="concat($label, 'A')"/>
      </xsl:with-param>
      <xsl:with-param name="value">
        <xsl:value-of select="$value"/>
      </xsl:with-param>
    </xsl:call-template>
  </xsl:template>
  
  <!-- Format4 -->
  <xsl:template name="Format4">
    <xsl:param name="label1"/>
    <xsl:param name="label2"/>
    <xsl:param name="label3"/>
    <xsl:param name="label4"/>
    <xsl:param name="value"/>
    <xsl:variable name="red">
      <xsl:value-of select="normalize-space(substring-before($value, ' '))"/>
    </xsl:variable>
    <xsl:variable name="after-red">
      <xsl:value-of select="substring($value, string-length($red)+2)"/>
    </xsl:variable>
    <xsl:variable name="green">
      <xsl:value-of select="substring-before($after-red, ' ')"/>
    </xsl:variable>
    <xsl:variable name="after-green">
      <xsl:value-of select="substring($after-red, string-length($green)+2)"/>
    </xsl:variable>
    <xsl:variable name="blue">
      <xsl:value-of select="substring-before($after-green, ' ')"/>
    </xsl:variable>
    <xsl:variable name="alpha">
      <xsl:value-of select="substring($after-green, string-length($blue)+2)"/>
    </xsl:variable>
    <xsl:value-of select="concat($Tabs4, $label1, ' = ', $red, '&#xA;')"/>
    <xsl:value-of select="concat($Tabs4, $label2, ' = ', $green, '&#xA;')"/>
    <xsl:value-of select="concat($Tabs4, $label3, ' = ', $blue, '&#xA;')"/>
    <xsl:value-of select="concat($Tabs4, $label4, ' = ', $alpha, '&#xA;')"/>
  </xsl:template>
  
  <!-- Format stencil operation -->
  <xsl:template name="FormatStencilOp">
    <xsl:param name="value"/>
    <xsl:choose>
      <xsl:when test="$value='KEEP'">
        <xsl:text>Keep</xsl:text>
      </xsl:when>
      <xsl:when test="$value='ZERO'">
        <xsl:text>Zero</xsl:text>
      </xsl:when>
      <xsl:when test="$value='REPLACE'">
        <xsl:text>Replace</xsl:text>
      </xsl:when>
      <xsl:when test="$value='INCR'">
        <xsl:text>IncrSat</xsl:text>
      </xsl:when>
      <xsl:when test="$value='DECR'">
        <xsl:text>DecrSat</xsl:text>
      </xsl:when>
      <xsl:when test="$value='INVERT'">
        <xsl:text>Invert</xsl:text>
      </xsl:when>
      <xsl:when test="$value='INCR_WRAP'">
        <xsl:text>Incr</xsl:text>
      </xsl:when>
      <xsl:when test="$value='DECR_WRAP'">
        <xsl:text>Decr</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$value"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Formats polygon mode -->
  <xsl:template name="FormatPolygonMode">
    <xsl:param name="value"/>
    <xsl:choose>
      <xsl:when test="$value='POINT'">
        <xsl:text>Point</xsl:text>
      </xsl:when>
      <xsl:when test="$value='LINE'">
        <xsl:text>Wireframce</xsl:text>
      </xsl:when>
      <xsl:when test="$value='FILL'">
        <xsl:text>Solid</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$value"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <!-- Formats a face-->
  <xsl:template name="FormatFace">
    <xsl:param name="value"/>
    <xsl:choose>
      <xsl:when test="$value='FRONT'">
        <xsl:text>Front</xsl:text>
      </xsl:when>
      <xsl:when test="$value='BACK'">
        <xsl:text>Back</xsl:text>
      </xsl:when>
      <xsl:when test="$value='FRONT_AND_BACK'">
        <xsl:text>Front_And_Back</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$value"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <!-- Formats a compare function -->
  <xsl:template name="FormatCompareFunc">
    <xsl:param name="value"/>
    <xsl:choose>
      <xsl:when test="$value='LEQUAL'">
        <xsl:text>LessEqual</xsl:text>
      </xsl:when>
      <xsl:when test="$value='GEQUAL'">
        <xsl:text>GreaterEqual</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$value"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- Formats a blend operation -->
  <xsl:template name="FormatBlendOp">
    <xsl:param name="value"/>
    <xsl:choose>
      <xsl:when test="$value='FUNC_ADD'">
        <xsl:text>Add</xsl:text>
      </xsl:when>
      <xsl:when test="$value='FUNC_SUBTRACT'">
        <xsl:text>Subtract</xsl:text>
      </xsl:when>
      <xsl:when test="$value='FUNC_REVERSE_SUBTRACT'">
        <xsl:text>RevSubtract</xsl:text>
      </xsl:when>
      <xsl:when test="$value='MIN'">
        <xsl:text>Min</xsl:text>
      </xsl:when>
      <xsl:when test="$value='MAX'">
        <xsl:text>Max</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$value"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
  
  <!-- Formats a blend function -->
  <xsl:template name="FormatBlendValue">
    <xsl:param name="value"/>
    <xsl:choose>
      <xsl:when test="$value='SRC_COLOR'">
        <xsl:text>SrcColor</xsl:text>
      </xsl:when>
      <xsl:when test="$value='ONE_MINUS_SRC_COLOR'">
        <xsl:text>InvSrcColor</xsl:text>
      </xsl:when>
      <xsl:when test="$value='SRC_ALPHA'">
        <xsl:text>SrcAlpha</xsl:text>
      </xsl:when>
      <xsl:when test="$value='ONE_MINUS_SRC_ALPHA'">
        <xsl:text>InvSrcAlpha</xsl:text>
      </xsl:when>
      <xsl:when test="$value='DEST_ALPHA'">
        <xsl:text>DestAlpha</xsl:text>
      </xsl:when>
      <xsl:when test="$value='ONE_MINUS_DEST_ALPHA'">
        <xsl:text>InvDestAlpha</xsl:text>
      </xsl:when>
      <xsl:when test="$value='DEST_COLOR'">
        <xsl:text>DestColor</xsl:text>
      </xsl:when>
      <xsl:when test="$value='ONE_MINUS_DEST_COLOR'">
        <xsl:text>InvDestColor</xsl:text>
      </xsl:when>
      <xsl:when test="$value='SRC_ALPHA_SATURATE'">
        <xsl:text>SrcAlphaSat</xsl:text>
      </xsl:when>
      <xsl:when test="$value='CONSTANT_COLOR'">
        <xsl:text>BlendFactor</xsl:text>
      </xsl:when>
      <xsl:when test="$value='ONE_MINUS_CONSTANT_COLOR'">
        <xsl:text>InvBlendFactor</xsl:text>
      </xsl:when>
      <xsl:when test="$value='CONSTANT_ALPHA'">
        <xsl:text>ConstantAlpha</xsl:text>
      </xsl:when>
      <xsl:when test="$value='ONE_MINUS_CONSTANT_ALPHA'">
        <xsl:text>InvConstantAlpha</xsl:text>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$value"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>

  <!-- 
  Sometimes COLLADA files will be missing value attributes
  when the default value is to be used. This will explicity 
  set the default value if its missing.
  -->
  <xsl:template name="GetValue">
    <xsl:param name="Value"/>
    <xsl:param name="Default"/>
    <xsl:choose>
      <xsl:when test="normalize-space($Value)=''">
        <xsl:value-of select="$Default"/>
      </xsl:when>
      <xsl:otherwise>
        <xsl:value-of select="$Value"/>
      </xsl:otherwise>
    </xsl:choose>
  </xsl:template>
</xsl:stylesheet>
