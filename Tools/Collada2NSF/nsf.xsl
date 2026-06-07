<?xml version="1.0" encoding="utf-8"?>
<xsl:stylesheet version="2.0"
                xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                xmlns:col="http://www.collada.org/2005/11/COLLADASchema">

  <xsl:strip-space elements="doc chapter section"/>
  <xsl:output method="text" indent="yes" standalone="yes" 
     encoding="iso-8859-1"
     omit-xml-declaration="yes"/>

  <!-- Upper and lower case translations -->
  <xsl:variable name="u" select="'ABCDEFGHIJKLMNOPQRSTUVWXYZ'"/>
  <xsl:variable name="l" select="'abcdefghijklmnopqrstuvwxyz'"/>

  <!-- Padding Variables for Text Formatting -->
  <xsl:variable name="Tab">
    <xsl:text>    </xsl:text>
  </xsl:variable>
  <xsl:variable name="Tabs2">
    <xsl:value-of select="concat($Tab, $Tab)"/>
  </xsl:variable>
  <xsl:variable name="Tabs3">
    <xsl:value-of select="concat($Tabs2, $Tab)"/>
  </xsl:variable>
  <xsl:variable name="Tabs4">
    <xsl:value-of select="concat($Tabs2, $Tabs2)"/>
  </xsl:variable>

  <xsl:include href="samplers.xsl"/>
  <xsl:include href="renderstates.xsl"/>
  <xsl:include href="attribs.xsl"/>
  <xsl:include href="constantmap.xsl"/>
  <xsl:include href="objects.xsl"/>


  <!-- Start of processing -->
  <xsl:template match="col:COLLADA">
    <xsl:apply-templates select="col:library_materials"/>
  </xsl:template>

  <!-- 
  library_materials
  Each material contains an effect_instance node with a url to the 
  actual effect. This template will locate that template in
  the library-effects section and process it.
  -->
  <xsl:template match="col:library_materials">
    <xsl:for-each select="col:material">
      <xsl:variable name="EffectId" select="substring-after(col:instance_effect/@url, '#')"/>
      <xsl:apply-templates select="/col:COLLADA/col:library_effects/col:effect[@id = $EffectId]">
        <xsl:with-param name="MaterialId" select="@id"/>
        <xsl:with-param name="MaterialName" select="@name"/>
      </xsl:apply-templates>
    </xsl:for-each>
  </xsl:template>

  <!-- Effect Templates -->
  <xsl:template match="col:effect">
    <xsl:param name="MaterialId"/>
    <xsl:param name="MaterialName"/>
    <xsl:text>// --------------------------------------------------------------------------&#xA;</xsl:text>
    <xsl:text>// This file has been generated from a COLLADA FX File. Do not edit manually.&#xA;</xsl:text>
    <xsl:text>// --------------------------------------------------------------------------&#xA;</xsl:text>
    <xsl:text>&#xA;</xsl:text>
    <xsl:value-of select="concat('NSFShader ', $MaterialName, '&#xA;{&#xA;')"/>
    <xsl:for-each select="col:profile_CG[@platform='PS3']">
      <xsl:call-template name="EffectDescription"/>
      <xsl:apply-templates select=".">
        <xsl:with-param name="MaterialId" select="$MaterialId"/>
      </xsl:apply-templates>
    </xsl:for-each>
    <xsl:text>}&#xA;</xsl:text>
  </xsl:template>

  <!-- Emit Description -->
  <xsl:template name="EffectDescription">
    <xsl:variable name="Details">
      <xsl:for-each select="col:newparam[@sid='description']/col:annotate">
        <xsl:sort data-type="number" select="substring-after(@name, 'detail')" order="ascending" />
        <xsl:variable name="Detail" select="col:string"/>
        <xsl:value-of select="concat($Tab, '&quot;', $Detail, '&quot;', '&#xA;')" />
      </xsl:for-each>
    </xsl:variable>
    <xsl:value-of select="concat($Tab, '&quot;', col:newparam[@sid='description']/col:string, '&quot;&#xA;')"/>
    <xsl:value-of select="$Details" />
    <xsl:text>&#xA;</xsl:text>
  </xsl:template>
  
  <!-- CG Profile -->
  <xsl:template match="col:profile_CG[@platform='PS3']">
    <xsl:param name="MaterialId"/>
    <xsl:call-template name="ObjectList"/>
    <xsl:call-template name="GlobalAttributeList">
      <xsl:with-param name="MaterialId" select="$MaterialId"/>
    </xsl:call-template>
    <xsl:call-template name="AttributeList">
      <xsl:with-param name="MaterialId" select="$MaterialId"/>
    </xsl:call-template>
    <xsl:call-template name="Implementation"/>
  </xsl:template>

  <!-- Implementation -->
  <xsl:template name="Implementation">
    <xsl:for-each select="col:technique">
      <xsl:apply-templates select="."/>
    </xsl:for-each>
  </xsl:template>

  <!-- Technique -->
  <xsl:template match="col:technique">
    <xsl:value-of select="concat($Tab, 'Implementation PS3CG', '&#xA;')"/>
    <xsl:value-of select="concat($Tab, '{&#xA;')"/>
    <xsl:call-template name="Requirements"/>
    <xsl:apply-templates select="col:pass"/>
    <xsl:value-of select="concat($Tab, '}&#xA;')"/>
  </xsl:template>

  <!-- Pass -->
  <xsl:template match="col:pass">
    <xsl:value-of select="concat($Tabs2, 'Pass ', @sid, '&#xA;')"/>
    <xsl:value-of select="concat($Tabs2, '{&#xA;')"/>
    <xsl:value-of select="concat($Tabs3, 'RenderStates&#xA;')"/>
    <xsl:value-of select="concat($Tabs3, '{&#xA;')"/>
    <xsl:apply-templates/>
    <xsl:value-of select="concat($Tabs3, '}&#xA;')"/>
    <xsl:value-of select="concat($Tabs3, 'VSProgram ')"/>
    <xsl:variable name="VertexShader" select="col:shader[@stage='VERTEX']"/>
    <xsl:variable name="FragmentShader" select="col:shader[@stage='FRAGMENT']"/>
    <xsl:call-template name="ShaderProgram">
      <xsl:with-param name="Shader" select="$VertexShader"/>
    </xsl:call-template>
    <xsl:call-template name="ConstantMap">
      <xsl:with-param name="Label" >VS_Constantmap</xsl:with-param>
      <xsl:with-param name="Shader" select="$VertexShader"/>
    </xsl:call-template>
    <xsl:for-each select="./col:shader">
      <xsl:call-template name="Sampler"/>
    </xsl:for-each>
    <xsl:value-of select="concat($Tabs3, 'PSProgram ')"/>
    <xsl:call-template name="ShaderProgram">
      <xsl:with-param name="Shader" select="$FragmentShader"/>
    </xsl:call-template>
    <xsl:call-template name="ConstantMap">
      <xsl:with-param name="Label" >PS_Constantmap</xsl:with-param>
      <xsl:with-param name="Shader" select="$FragmentShader"/>
    </xsl:call-template>
    <xsl:value-of select="concat($Tabs2, '}&#xA;&#xA;')"/>
  </xsl:template>

  <!-- Process Shader Program -->
  <xsl:template name="ShaderProgram">
    <xsl:param name="Shader"/>
    <xsl:variable name="Filename" select="../../col:include[1]/@url"/>
    <xsl:variable name="Entry" select="$Shader/col:name"/>
    <xsl:value-of select="concat('&quot;', $Filename, '&quot;', ' ', $Entry, '&#xA;')"/>
  </xsl:template>

  <!-- Implementation Requirments -->
  <xsl:template name="Requirements">
    <xsl:value-of select="concat($Tabs2, 'Requirements &#xA;', $Tabs2, '{&#xA;')"/>
    <xsl:value-of select="concat($Tabs3, 'Platform = PS3', '&#xA;')"/>
    <xsl:value-of select="concat($Tabs3, 'UsesNIRenderState = true', '&#xA;')"/>
    <xsl:value-of select="concat($Tabs3, 'UsesNILighting = false', '&#xA;')"/>
    <xsl:variable name="HasBones" select="col:annotate[@name='BonesPerPartition']/col:int"/>
    <xsl:choose>
      <xsl:when test="$HasBones!=''">
        <xsl:value-of select="concat($Tabs3, 'BonesPerPartition = ', $HasBones, '&#xA;')"/>
      </xsl:when>
    </xsl:choose>
    <xsl:value-of select="concat($Tabs2, '}', '&#xA;')"/>
  </xsl:template>

  <!-- Ignore extra -->
  <xsl:template match="col:extra">
  </xsl:template>
  
  <!-- Filter out unprocessed data -->
  <xsl:template match="text()|@*">
  </xsl:template>
</xsl:stylesheet>
